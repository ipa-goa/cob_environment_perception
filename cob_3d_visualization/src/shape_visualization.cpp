/****************************************************************
 *
 * Copyright (c) 2010
 *
 * Fraunhofer Institute for Manufacturing Engineering
 * and Automation (IPA)
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Project name: care-o-bot
 * ROS stack name: cob_environment_perception_intern
 * ROS package name: cob_3d_mapping_common
 * Description:
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Author: Waqas Tanveer, email:Waqas.Tanveer@ipa.fhg.de
 * Supervised by: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 *
 * Date of creation: 04/2012
 * ToDo:
 *
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * * Neither the name of the Fraunhofer Institute for Manufacturing
 * Engineering and Automation (IPA) nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License LGPL as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License LGPL for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License LGPL along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************/

//##################
//#### includes ####


#include <cob_3d_visualization/shape_marker.h>
#include <cob_3d_visualization/table_marker.h>
#include <cob_3d_visualization/shape_visualization.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
//#include <Eigen/Core>
//#include <cob_3d_mapping_geometry_map/geometry_map.h>
#define PI 3.14159265

using namespace cob_3d_mapping;

/**
 * @brief Callback for feedback subscriber for getting the transformation of moved markers
 *
 * @param feedback subscribed from geometry_map/map/feedback
 */
void ShapeVisualization::setShapePosition(const visualization_msgs::InteractiveMarkerFeedbackConstPtr& feedback)//,const cob_3d_mapping_msgs::Shape& shape)
{

  cob_3d_mapping_msgs::ShapeArray map_msg;
  map_msg.header.frame_id="/map";
  map_msg.header.stamp = ros::Time::now();

  int shape_id,index;
  index=-1;
  stringstream name(feedback->marker_name);

  Eigen::Quaternionf quat;

  Eigen::Matrix3f rotationMat;
  Eigen::MatrixXf rotationMatInit;

  Eigen::Vector3f normalVec;
  Eigen::Vector3f normalVecNew;
  Eigen::Vector3f newCentroid;
  Eigen::Matrix4f transSecondStep;


  if (feedback->marker_name != "Text"){
    name >> shape_id ;
    cob_3d_mapping::Polygon p;

    for(int i=0;i<sha.shapes.size();++i)
    {
    	if (sha.shapes[i].id == shape_id)
	{
		index = i;
	}

    }


    // temporary fix.
    //do nothing if index of shape is not found
    // this is not supposed to occur , but apparently it does
    if(index==-1){

    //std::cout<<"BROKEN SHAPE ID"<<shape_id<<"\n";
    //for(int i=0;i<sha.shapes.size();++i)
    //{
    //std::cout<<sha.shapes[i].id<<"\n";
    //}
    return;
	}


std::cout<<"index"<<index<<"\n";
    cob_3d_mapping::fromROSMsg (sha.shapes.at(index), p);

    if (feedback->event_type == 2 && feedback->menu_entry_id == 5){
      quatInit.x() = (float)feedback->pose.orientation.x ;           //normalized
      quatInit.y() = (float)feedback->pose.orientation.y ;
      quatInit.z() = (float)feedback->pose.orientation.z ;
      quatInit.w() = (float)feedback->pose.orientation.w ;

      oldCentroid (0) = (float)feedback->pose.position.x ;
      oldCentroid (1) = (float)feedback->pose.position.y ;
      oldCentroid (2) = (float)feedback->pose.position.z ;

      quatInit.normalize() ;

      rotationMatInit = quatInit.toRotationMatrix() ;

      transInit.block(0,0,3,3) << rotationMatInit ;
      transInit.col(3).head(3) << oldCentroid(0) , oldCentroid(1), oldCentroid(2) ;
      transInit.row(3) << 0,0,0,1 ;

      transInitInv = transInit.inverse() ;
      Eigen::Affine3f affineInitFinal (transInitInv) ;
      affineInit = affineInitFinal ;

      std::cout << "transInit : " << "\n"    << affineInitFinal.matrix() << "\n" ;
    }

    if (feedback->event_type == 5){
      /* the name of the marker is arrows_shape_.id, we need to erase the "arrows_" part */
      //string strName(feedback->marker_name);
      //strName.erase(strName.begin(),strName.begin()+7);
//      stringstream name(strName);
	stringstream name(feedback->marker_name);

      name >> shape_id ;
      cob_3d_mapping::Polygon p;
      cob_3d_mapping::fromROSMsg (sha.shapes.at(index), p);

      quat.x() = (float)feedback->pose.orientation.x ;           //normalized
      quat.y() = (float)feedback->pose.orientation.y ;
      quat.z() = (float)feedback->pose.orientation.z ;
      quat.w() = (float)feedback->pose.orientation.w ;

      quat.normalize() ;

      rotationMat = quat.toRotationMatrix() ;

      normalVec << sha.shapes.at(index).params[0],                   //normalized
          sha.shapes.at(index).params[1],
          sha.shapes.at(index).params[2];

      newCentroid << (float)feedback->pose.position.x ,
          (float)feedback->pose.position.y ,
          (float)feedback->pose.position.z ;


      transSecondStep.block(0,0,3,3) << rotationMat ;
      transSecondStep.col(3).head(3) << newCentroid(0) , newCentroid(1), newCentroid(2) ;
      transSecondStep.row(3) << 0,0,0,1 ;

      Eigen::Affine3f affineSecondStep(transSecondStep) ;

      std::cout << "transfrom : " << "\n"    << affineSecondStep.matrix() << "\n" ;

      Eigen::Affine3f affineFinal(affineSecondStep*affineInit) ;
      Eigen::Matrix4f matFinal = (transSecondStep*transInitInv) ;

      normalVecNew    = (matFinal.block(0,0,3,3))* normalVec;
      //      newCentroid  = transFinal *OldCentroid ;


      sha.shapes.at(index).centroid.x = newCentroid(0) ;
      sha.shapes.at(index).centroid.y = newCentroid(1) ;
      sha.shapes.at(index).centroid.z = newCentroid(2) ;


      sha.shapes.at(index).params[0] = normalVecNew(0) ;
      sha.shapes.at(index).params[1] = normalVecNew(1) ;
      sha.shapes.at(index).params[2] = normalVecNew(2) ;


      std::cout << "transfromFinal : " << "\n"    << affineFinal.matrix() << "\n" ;

      pcl::PointCloud<pcl::PointXYZ> pc;
      pcl::PointXYZ pt;
      sensor_msgs::PointCloud2 pc2;

      for(unsigned int j=0; j<p.contours.size(); j++)
      {
        for(unsigned int k=0; k<p.contours[j].size(); k++)
        {
          p.contours[j][k] = affineFinal * p.contours[j][k];
          pt.x = p.contours[j][k][0] ;
          pt.y = p.contours[j][k][1] ;
          pt.z = p.contours[j][k][2] ;
          pc.push_back(pt) ;
        }
      }

      pcl::toROSMsg (pc, pc2);
      sha.shapes.at(index).points.clear() ;
      sha.shapes.at(index).points.push_back (pc2);

      // uncomment when using test_shape_array

      //      for(unsigned int i=0;i<sha.shapes.size();i++){
      //        map_msg.header = sha.shapes.at(i).header ;
      //        map_msg.shapes.push_back(sha.shapes.at(i)) ;
      //      }
      //      shape_pub_.publish(map_msg);

      // end uncomment

      modified_shapes_.shapes.push_back(sha.shapes.at(index));
    }

  }
}
/**
 * @brief Feedback callback for Apply map modifications menu entry
 *
 * @param feedback feedback from rviz when the Apply map modifications menu entry of the text is changed
 */
void ShapeVisualization::applyModifications(const visualization_msgs::InteractiveMarkerFeedbackConstPtr& feedback)
{
  cob_3d_mapping_msgs::ModifyMap::Request req ;
  cob_3d_mapping_msgs::ModifyMap::Response res;

  /*****Modify shapes*****/
  if (!modified_shapes_.shapes.empty()){
    //    ROS_INFO("modify action...");
    for(unsigned int i=0;i<modified_shapes_.shapes.size();i++){
      req.InMap.shapes.push_back(modified_shapes_.shapes.at(i)) ;
    }
    req.action = cob_3d_mapping_msgs::ModifyMapRequest::MODIFY ;
    std ::cout << "size of request: " << req.InMap.shapes.size() << "\n" ;
    if (ros::service::call("geometry_map/modify_map",req,res))
    {
      std::cout << "calling ModifyMap service..." << "\n" ;
    }

    while (!req.InMap.shapes.empty()){
      req.InMap.shapes.pop_back() ;
      modified_shapes_.shapes.pop_back() ;
    }
  }

  /*****Delete shapes*****/
  if (!deleted_markers_indices_.empty()){

    std::cout<< "deleted_markers_indices_ size : " << deleted_markers_indices_.size() << "\n" ;
    //    ROS_INFO("delete action...");
    req.action = cob_3d_mapping_msgs::ModifyMapRequest::DELETE ;
    for(unsigned int i=0;i<deleted_markers_indices_.size();i++){
      req.InMap.shapes.push_back(sha.shapes.at(deleted_markers_indices_.at(i))) ;
    }

    if (ros::service::call("geometry_map/modify_map",req,res))
    {
      std::cout << "calling ModifyMap service..." << "\n" ;
    }

    while (!req.InMap.shapes.empty()){
      req.InMap.shapes.pop_back() ;
      deleted_markers_indices_.pop_back() ;

    }
    std::cout<< "deleted_markers_indices_ size : " << deleted_markers_indices_.size() << "\n" ;
    std::cout << "req size" << req.InMap.shapes.size() << "\n" ;
  }

  im_server_->applyChanges() ;
}
/**
 * @brief Feedback callback for Reset all Controls menu entry
 *
 * @param feedback feedback from rviz when the Reset all Controls menu entry of the text is changed
 */
void ShapeVisualization::resetAll(const visualization_msgs::InteractiveMarkerFeedbackConstPtr& feedback)
{
  stringstream aa;
  std::cout <<"interacted_shapes_.size() = " << interacted_shapes_.size() <<"\n" ;

  for (unsigned int i=0; i< interacted_shapes_.size();i++)
  {
    unsigned int id = interacted_shapes_[i];
    for (unsigned int j=0; j<v_sm_.size(); j++)
    {
      if(id == v_sm_[j]->getID())
        v_sm_[j]->resetMarker();
      //        interacted_shapes_.pop_back();
    }

  }
  for (unsigned int i=0; i< moved_shapes_indices_.size();i++){

    unsigned int id = moved_shapes_indices_[i];
    for (unsigned int j=0; j<v_sm_.size(); j++)
    {
      if(id == v_sm_[j]->getID())
        v_sm_[j]->hideArrows(0);
    }
  }
  for (unsigned int i=0; i< deleted_markers_indices_.size();i++){
    unsigned int id = deleted_markers_indices_[i];
    for (unsigned int j=0; j<v_sm_.size(); j++)
    {
      if(id == v_sm_[j]->getID())
        deleted_markers_indices_.erase(deleted_markers_indices_.begin()+i);
        v_sm_[j]->createInteractiveMarker();
    }
  }


  interacted_shapes_.clear();
  deleted_markers_indices_.clear() ;
  moved_shapes_indices_.clear() ;
  modified_shapes_.shapes.clear() ;
  im_server_->applyChanges ();

}
/**
 * @brief creats a text for applying controls on all of the markers
 **/
void ShapeVisualization::moreOptions()
{
  optionMenu();
  visualization_msgs::Marker Text;

  Text.type = visualization_msgs::Marker::TEXT_VIEW_FACING;
  Text.action = visualization_msgs::Marker::ADD;
  Text.lifetime = ros::Duration ();
  Text.header.frame_id = "/map" ;

  Text.id = 0;
  Text.ns = "text";

  Text.text = "Click here for more options" ;
  // Scale
  Text.scale.x = 0.3;
  Text.scale.y = 0.3;
  Text.scale.z = 0.3;

  // Pose
  Text.pose.position.x = 0;
  Text.pose.position.y = 0;
  Text.pose.position.z = 0.5;

  Text.pose.orientation.x = 0;
  Text.pose.orientation.y = 0;
  Text.pose.orientation.z = 0;
  Text.pose.orientation.w = 1;



  Text.color.r = 1;
  Text.color.g = 1;
  Text.color.b = 1;
  Text.color.a = 1;

  /*Interactive Marker for the Text*/

  visualization_msgs::InteractiveMarker imarkerText;
  visualization_msgs::InteractiveMarkerControl im_ctrl_text_ ;
  im_ctrl_text_.always_visible = true ;
  im_ctrl_text_.interaction_mode = visualization_msgs::InteractiveMarkerControl::BUTTON;
  imarkerText.name = "Text" ;
  imarkerText.header  = Text.header ;
  im_ctrl_text_.markers.push_back(Text);
  imarkerText.controls.push_back(im_ctrl_text_);

  im_server_->insert (imarkerText);
  menu_handler_for_text_.apply (*im_server_,imarkerText.name);

}
/**
 * @brief Feedback callback for All Normals Controls menu entry
 *
 * @param feedback feedback from rviz when the All Normals menu entry of the text is changed
 */
void ShapeVisualization::displayAllNormals(const visualization_msgs::InteractiveMarkerFeedbackConstPtr& feedback) {

  interactive_markers::MenuHandler::CheckState check_state;

  menu_handler_for_text_.getCheckState (feedback->menu_entry_id, check_state);
  if (check_state == interactive_markers::MenuHandler::UNCHECKED)
  {
    //ROS_INFO(" entry state changed ");
    ROS_INFO ("Displaying all Normals...");
    menu_handler_for_text_.setCheckState (feedback->menu_entry_id, interactive_markers::MenuHandler::CHECKED);

    for (unsigned int j=0; j<v_sm_.size(); j++)
    {
    std::cout<<j<<std::endl;
      v_sm_[j]->displayNormal();
    }
  }
  else if (check_state == interactive_markers::MenuHandler::CHECKED)
  {
    //ROS_INFO(" entry state changed ");
    menu_handler_for_text_.setCheckState (feedback->menu_entry_id, interactive_markers::MenuHandler::UNCHECKED);
    ROS_INFO ("Deleting all Normals...");
    for (unsigned int j=0; j<v_sm_.size(); j++)
    {
      v_sm_[j]->hideNormal(0);
    }
  }
  menu_handler_for_text_.reApply (*im_server_);
  im_server_->applyChanges ();
}
/**
 * @brief Feedback callback for All Centroids Controls menu entry
 *
 * @param feedback feedback from rviz when the All Centroids menu entry of the text is changed
 */
void
ShapeVisualization::displayAllCentroids (const visualization_msgs::InteractiveMarkerFeedbackConstPtr& feedback)
{

  interactive_markers::MenuHandler::CheckState check_state;
  menu_handler_for_text_.getCheckState (feedback->menu_entry_id, check_state);

  if (check_state == interactive_markers::MenuHandler::UNCHECKED)
  {
    menu_handler_for_text_.setCheckState (feedback->menu_entry_id, interactive_markers::MenuHandler::CHECKED);
    for (unsigned int j=0; j<v_sm_.size(); j++)
    {
      v_sm_[j]->displayCentroid();
    }
  }
  else if (check_state == interactive_markers::MenuHandler::CHECKED)
  {
    menu_handler_for_text_.setCheckState (feedback->menu_entry_id, interactive_markers::MenuHandler::UNCHECKED);
    for (unsigned int i=0; i< sha.shapes.size();i++)
    {

      //ROS_INFO(" entry state changed ");
      for (unsigned int j=0; j<v_sm_.size(); j++)
      {
        v_sm_[j]->hideCentroid(0);
      }
    }
  }
  menu_handler_for_text_.reApply (*im_server_);
  im_server_->applyChanges ();
}
/**
 * @brief Feedback callback for All Contours Controls menu entry
 *
 * @param feedback feedback from rviz when the All Contours menu entry of the text is changed
 */
void
ShapeVisualization::displayAllContours (const visualization_msgs::InteractiveMarkerFeedbackConstPtr& feedback){

  interactive_markers::MenuHandler::CheckState check_state;
  menu_handler_for_text_.getCheckState (feedback->menu_entry_id, check_state);

  if (check_state == interactive_markers::MenuHandler::UNCHECKED)
  {
    menu_handler_for_text_.setCheckState (feedback->menu_entry_id, interactive_markers::MenuHandler::CHECKED);
    for (unsigned int j=0; j<v_sm_.size(); j++)
    {
      v_sm_[j]->displayContour();
    }
  }
  else if (check_state == interactive_markers::MenuHandler::CHECKED)
  {
    menu_handler_for_text_.setCheckState (feedback->menu_entry_id, interactive_markers::MenuHandler::UNCHECKED);
    for (unsigned int i=0; i< sha.shapes.size();i++)
    {

      //ROS_INFO(" entry state changed ");
      for (unsigned int j=0; j<v_sm_.size(); j++)
      {
        v_sm_[j]->hideContour(0);
      }
    }
  }
  menu_handler_for_text_.reApply (*im_server_);
  im_server_->applyChanges ();
}
/**
 * @brief Create menu entries for the text
 */
void ShapeVisualization::optionMenu() {

  //  ROS_INFO("Creating menu for the text...") ;

  interactive_markers::MenuHandler::EntryHandle eh_1, eh_2 , eh_3 ,eh_4 ,eh_5, eh_6;

  eh_1 = menu_handler_for_text_.insert ("Menu");
  eh_2 = menu_handler_for_text_.insert (eh_1, "All Normals",boost::bind (&ShapeVisualization::displayAllNormals, this, _1));
  eh_3 = menu_handler_for_text_.insert (eh_1, "All Centroids",boost::bind (&ShapeVisualization::displayAllCentroids, this, _1));
  eh_4 = menu_handler_for_text_.insert (eh_1, "All Contours",boost::bind (&ShapeVisualization::displayAllContours, this, _1));
  //  eh_4 = menu_handler_for_text_.insert (eh_1, "Find tables",boost::bind (&ShapeVisualization::findTables, this, _1));
  eh_5 = menu_handler_for_text_.insert (eh_1, "Apply map modifications",boost::bind (&ShapeVisualization::applyModifications, this, _1));
  eh_6 = menu_handler_for_text_.insert (eh_1, "Reset all Controls",boost::bind (&ShapeVisualization::resetAll, this, _1));

  menu_handler_for_text_.setVisible (eh_1, true);
  menu_handler_for_text_.setCheckState (eh_1, interactive_markers::MenuHandler::NO_CHECKBOX);
  menu_handler_for_text_.setVisible (eh_2, true);
  menu_handler_for_text_.setCheckState (eh_2, interactive_markers::MenuHandler::UNCHECKED);
  menu_handler_for_text_.setVisible (eh_3, true);
  menu_handler_for_text_.setCheckState (eh_3, interactive_markers::MenuHandler::UNCHECKED);
  menu_handler_for_text_.setVisible (eh_4, true);
  menu_handler_for_text_.setCheckState (eh_4, interactive_markers::MenuHandler::UNCHECKED);
  menu_handler_for_text_.setVisible (eh_5, true);
  menu_handler_for_text_.setCheckState (eh_5, interactive_markers::MenuHandler::NO_CHECKBOX);
  menu_handler_for_text_.setVisible (eh_6, true);
  menu_handler_for_text_.setCheckState (eh_6, interactive_markers::MenuHandler::NO_CHECKBOX);


}

/**
 * @brief Callback for shape array messages
 *
 * @param sa received shape array message
 */
void
ShapeVisualization::shapeArrayCallback (const cob_3d_mapping_msgs::ShapeArrayPtr& sa)
{
  //  ctr_for_shape_indexes = 0 ;
  v_sm_.clear();
  sha.shapes.clear() ;
  im_server_->applyChanges();
  ROS_INFO("shape array with %d shapes received", sa->shapes.size());

  for (unsigned int i = 0; i < sa->shapes.size (); i++)
  {
    sha.shapes.push_back(sa->shapes[i]);
    sha.shapes[i].id = sa->shapes[i].id;

    
    boost::shared_ptr<ShapeMarker> sm(new ShapeMarker(im_server_, sa->shapes[i],moved_shapes_indices_,interacted_shapes_,deleted_markers_indices_));
    v_sm_.push_back(sm);
  }
//  im_server_->applyChanges(); //update changes
}

int
main (int argc, char** argv)
{
  ros::init (argc, argv, "shape_visualization");
  ROS_INFO("shape_visualization node started....");
  ShapeVisualization sv;
  ros::spin();
}
