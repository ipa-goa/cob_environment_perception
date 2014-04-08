/*!
 *****************************************************************
 * \file
 *
 * \note
 *   Copyright (c) 2012 \n
 *   Fraunhofer Institute for Manufacturing Engineering
 *   and Automation (IPA) \n\n
 *
 *****************************************************************
 *
 * \note
 *  Project name: care-o-bot
 * \note
 *  ROS stack name: cob_vision
 * \note
 *  ROS package name: cob_env_model
 *
 * \author
 *  Author: Georg Arbeiter, email:georg.arbeiter@ipa.fhg.de
 * \author
 *  Supervised by:
 *
 * \date Date of creation: 02/2011
 *
 * \brief
 * Description:
 *
 * ToDo:
 *
 *****************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     - Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer. \n
 *     - Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution. \n
 *     - Neither the name of the Fraunhofer Institute for Manufacturing
 *       Engineering and Automation (IPA) nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission. \n
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

#ifndef IMPL_FIELDOFVIEWSEGMENTATION_H_
#define IMPL_FIELDOFVIEWSEGMENTATION_H_

#include <cob_3d_mapping_point_map/field_of_view_segmentation.h>



template <typename PointT>
void ipa_env_model::FieldOfViewSegmentation<PointT>::segment(pcl::PointIndices &indices,
		const Eigen::Vector3d &n_up, const Eigen::Vector3d &n_down, const Eigen::Vector3d &n_right, const Eigen::Vector3d &n_left, const Eigen::Vector3d &n_origin, const Eigen::Vector3d &n_max_range)
{
	indices.header = input_->header;
	for(unsigned int i = 0; i<input_->points.size(); i++)
	{
		const PointT* curPoint = &input_->points[i];
		if(n_up(0)*(curPoint->x-n_origin(0))+n_up(1)*(curPoint->y-n_origin(1))+n_up(2)*(curPoint->z-n_origin(2)) < 0 &&
				n_down(0)*(curPoint->x-n_origin(0))+n_down(1)*(curPoint->y-n_origin(1))+n_down(2)*(curPoint->z-n_origin(2)) < 0 &&
				n_right(0)*(curPoint->x-n_origin(0))+n_right(1)*(curPoint->y-n_origin(1))+n_right(2)*(curPoint->z-n_origin(2)) < 0 &&
				n_left(0)*(curPoint->x-n_origin(0))+n_left(1)*(curPoint->y-n_origin(1))+n_left(2)*(curPoint->z-n_origin(2)) < 0 /*&&
				n_max_range(0)*(curPoint->x-n_origin(0))+n_max_range(1)*(curPoint->y-n_origin(1))+n_max_range(2)*(curPoint->z-n_origin(2)) < 0*/)
		{
			indices.indices.push_back(i);
			//std::cout << "Point "  << *curPoint << " is in FOV" << std::endl;
		}
	}
}


#define PCL_INSTANTIATE_FieldOfViewSegmentation(T) template class ipa_env_model::FieldOfViewSegmentation<T>;

#endif /* IMPL_FIELDOFVIEWSEGMENTATION_H_ */