/****************************************************************
 *
 * Copyright (c) 2011
 *
 * Fraunhofer Institute for Manufacturing Engineering
 * and Automation (IPA)
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Project name: care-o-bot
 * ROS stack name: cob_environment_perception
 * ROS package name: cob_3d_mapping_common
 * Description:
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Author: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 * Supervised by: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 *
 * Date of creation: 03/2012
 * ToDo:
 *
 *
 *
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Fraunhofer Institute for Manufacturing
 *       Engineering and Automation (IPA) nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License LGPL as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License LGPL for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License LGPL along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************/

#ifndef POLYGON_H_
#define POLYGON_H_

#include "cob_3d_mapping_common/shape.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
extern "C" {
#include "cob_3d_mapping_common/gpc.h"
}
#include <fstream>

//#include "cob_3d_mapping_common/stop_watch.h"

namespace cob_3d_mapping
{

struct merge_config {
  double angle_thresh ;
  float  d_thresh;
  std::string weighting_method;
} ;


//Polygon Class, derived from Shape class


  class Polygon : public Shape
  {
  public:
    Polygon()
      : normal(Eigen::Vector3f::Zero())
      , d(0.0)
      , transform_from_world_to_plane(Eigen::Affine3f::Identity())
      , merge_weight_(1.0)
    { }

  //##########methods for instantiation##############
  virtual void computeAttributes(const Eigen::Vector3f &new_normal, const Eigen::Vector4f & new_centroid);
  virtual void transform2tf(const Eigen::Affine3f& trafo);


  //###########methods for merging##################
  virtual void isMergeCandidate(std::vector< boost::shared_ptr<Polygon> >& poly_vec,merge_config& config, std::vector<int>& intersections);
  bool isMergeCandidate_intersect(Polygon& p_map);
  virtual void merge(std::vector< boost::shared_ptr<Polygon> >& poly_vec);
  void merge_union(std::vector< boost::shared_ptr<Polygon> >& poly_vec, boost::shared_ptr<Polygon>&  p_average);
  void assignWeight();
  virtual void applyWeighting(const std::vector< boost::shared_ptr<Polygon> >& poly_vec , boost::shared_ptr<Polygon> & p_average );
  void GpcStructureUsingMap(const Eigen::Affine3f& external_trafo,gpc_polygon* gpc_p);
  void GpcStructure( gpc_polygon* gpc_p);

  //#######methods for calculation#####################

  void computeCentroid();
  void computeAnchor(int& index_i,int&index_);
  void computeAnchor(const std::vector<std::vector<Eigen::Vector3f> >& in_contours,int& index_i,int&index_j);

  double computeArea();   // http://paulbourke.net/geometry/polyarea/
  double computeArea3d();
  void getTransformationFromPlaneToWorld(const Eigen::Vector3f &normal,const Eigen::Vector3f &origin, Eigen::Affine3f &transformation);
  void getTransformationFromPlaneToWorld(const Eigen::Vector3f z_axis,const Eigen::Vector3f &normal,const Eigen::Vector3f &origin, Eigen::Affine3f &transformation);
  std::vector<std::vector<Eigen::Vector3f> > getTransformedContours(const Eigen::Affine3f& trafo);
  void TransformContours(const Eigen::Affine3f& trafo);
  void computePoseAndBoundingBox(Eigen::Affine3f& pose, Eigen::Vector4f& min_pt, Eigen::Vector4f& max_pt);


  //#############debugging methods#######################

  void debug_output(std::string name);


  //########## member variables#################
  //needed for 32-bit systems: see http://eigen.tuxfamily.org/dox/TopicStructHavingEigenMembers.html
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  std::vector<std::vector<Eigen::Vector3f> > contours;
  Eigen::Vector3f normal;
  double d;
  Eigen::Affine3f transform_from_world_to_plane;
  std::vector<bool> holes;
  double merge_weight_;
  merge_config merge_settings_;



private:




};

typedef boost::shared_ptr<Polygon> PolygonPtr;
}

#endif /* POLYGON_H_ */
