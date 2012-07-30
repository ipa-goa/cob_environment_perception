/*
 * qppf_node.cpp
 *
 *  Created on: 21.07.2012
 *      Author: josh
 */



// ROS includes
#include <ros/ros.h>
#include <pluginlib/class_list_macros.h>
#include <pcl_ros/pcl_nodelet.h>
#include <pcl/point_types.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_cloud.h>

#include <cob_3d_segmentation/quad_regression/quad_regression.h>
#include <cob_3d_mapping_msgs/CurvedPolygon_Array.h>
#include <cob_3d_mapping_msgs/FilterObject.h>

#include <actionlib/server/simple_action_server.h>
#include <cob_3d_segmentation/ObjectWatchGoal.h>
#include <cob_3d_segmentation/ObjectWatchFeedback.h>
#include <cob_3d_segmentation/ObjectWatchAction.h>
#include <geometry_msgs/PoseStamped.h>

class As_Node
{
protected:
  ros::NodeHandle n_;
public:
  As_Node() {
  }

  virtual ~As_Node() {}

  virtual void onInit()=0;

  void start() {

  }
};

class As_Nodelet : public  pcl_ros::PCLNodelet
{
protected:
  ros::NodeHandle n_;
public:
  As_Nodelet() {
  }

  virtual ~As_Nodelet() {}

  void start() {
    PCLNodelet::onInit();
    n_ = getNodeHandle();
  }
};

template <typename Point, typename PointLabel, typename Parent>
class QPPF_Node : public Parent
{
  typedef pcl::PointCloud<Point> PointCloud;

  ros::Subscriber point_cloud_sub_;
  ros::Publisher  curved_pub_, shapes_pub_;

  Segmentation::Segmentation_QuadRegression<Point, PointLabel> seg_;

  std::vector<cob_3d_mapping_msgs::FilterObject> filter_;

  actionlib::SimpleActionServer<cob_3d_segmentation::ObjectWatchAction> as_;
  cob_3d_segmentation::ObjectWatchGoalConstPtr  goal_;

public:
  // Constructor
  QPPF_Node(const std::string &name):
    as_(this->n_, name, boost::bind(&QPPF_Node::setGoal, this, _1), false)
  {
  }

  virtual ~QPPF_Node()
  {}

  void onInit() {
    this->start();

    ros::NodeHandle *n=&(this->n_);
    point_cloud_sub_ = this->n_.subscribe("/camera/depth/points", 1, &QPPF_Node<Point, PointLabel, Parent>::pointCloudSubCallback, this);
    curved_pub_ = n->advertise<cob_3d_mapping_msgs::CurvedPolygon_Array>("/curved_polygons", 1);
    shapes_pub_ = n->advertise<cob_3d_mapping_msgs::ShapeArray>("/shapes_array", 1);

    as_.start();
  }

  void setGoal(const cob_3d_segmentation::ObjectWatchGoalConstPtr &goal)
  {
    as_.setPreempted();

    if(!goal) {
      goal_.reset();
      return;
    }

    if(goal->widths.size()!=goal->heights.size())
    {
      ROS_ERROR("malformed goal message (|widths|!=|heights|): aborting...");
      goal_.reset();
    }
    else if(goal->widths.size()==0)
    {
      ROS_DEBUG("stopping segment filtering");
      goal_.reset();
    }
    else
      goal_ = goal;
  }

  void
  pointCloudSubCallback(const boost::shared_ptr<const PointCloud>& pc_in)
  {
    ROS_DEBUG("segmentation: point cloud callback");

    seg_.setInputCloud(pc_in);
    seg_.compute();
    if(goal_)
    {
      cob_3d_segmentation::ObjectWatchFeedback feedback;
      for(size_t i=0; i<seg_.getPolygons().size(); i++)
      {
        Eigen::Matrix3f P;
        Eigen::Vector3f origin;
        float h, w;
        if(seg_.getPolygons()[i].getPose(P,origin,h,w))
        {
          std::cerr<<"z: "<<origin(2)<<"  ";
          std::cerr<<"w: "<<w<<"  ";
          std::cerr<<"h: "<<h<<"\n";
          for(size_t j=0; j<goal_->heights.size(); j++)
            if( std::abs(h-goal_->heights[j])<0.2f*goal_->heights[j]  && std::abs(w-goal_->widths[j])<0.2f*goal_->widths[j]) {
              ROS_INFO("found");
              std::cout<<"P\n"<<P<<"\n";
              std::cout<<"origin\n"<<origin<<"\n";

              geometry_msgs::PoseStamped p;
              p.header = pc_in->header;
              Eigen::Quaternionf Q(P);
              p.pose.orientation.x = Q.x();
              p.pose.orientation.y = Q.y();
              p.pose.orientation.z = Q.z();
              p.pose.orientation.w = Q.w();
              p.pose.position.x = origin(0);
              p.pose.position.y = origin(1);
              p.pose.position.z = origin(2);
              feedback.objs.push_back(p);
            }
        }
      }
      as_.publishFeedback(feedback);
    }
    if(shapes_pub_.getNumSubscribers()>0)
    {
      cob_3d_mapping_msgs::ShapeArray sa = seg_;
      sa.header.stamp = pc_in->header.stamp;
      shapes_pub_.publish(sa);
    }
    if(curved_pub_.getNumSubscribers()>0)
    {
      cob_3d_mapping_msgs::CurvedPolygon_Array cpa;
      for(size_t i=0; i<seg_.getPolygons().size(); i++)
      {
        cob_3d_mapping_msgs::CurvedPolygon cp;
        seg_.getPolygons()[i].toRosMsg(&cp,pc_in->header.stamp);
        cpa.polygons.push_back(cp);
      }
      cpa.header = pc_in->header;
      curved_pub_.publish(cpa);
    }
  }
};

#ifdef COMPILE_NODELET

typedef QPPF_Node<pcl::PointXYZ,pcl::PointXYZRGB,As_Nodelet> QPPF_XYZ;

PLUGINLIB_DECLARE_CLASS(cob_3d_segmentation, QPPF_XYZ, QPPF_Node_XYZ, nodelet::Nodelet)

#else

int main(int argc, char **argv) {
  ros::init(argc, argv, "qppf");

  QPPF_Node<pcl::PointXYZ,pcl::PointXYZRGB,As_Node> sn("qppf");
  sn.onInit();

  ros::spin();

  return 0;
}

#endif
