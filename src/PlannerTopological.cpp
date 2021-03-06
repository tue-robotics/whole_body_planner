#include "whole_body_planner/PlannerTopological.h"

#include <visualization_msgs/MarkerArray.h>

PlannerTopological::PlannerTopological()
{
    ROS_INFO("Initializing plannertopological");
    current_state_ = "reset";

    /// Marker array publisher
    ros::NodeHandle n("~");
    marker_array_pub_ = n.advertise<visualization_msgs::MarkerArray>("topological_motion_constraints", 1);
    ROS_INFO("Initialized plannertopological");
}

PlannerTopological::~PlannerTopological()
{
    marker_array_pub_.shutdown();
}

bool PlannerTopological::computeConstraints(const amigo_whole_body_controller::ArmTaskGoal& goal_constraint, std::vector<amigo_whole_body_controller::ArmTaskGoal>& constraints)
{
    //constraints.resize(1);
    //constraints[0] = goal_constraint;

    /// Update the connectivitygraph according to the goal
    connectivity_graph_.updateGraph(goal_constraint);

    //displayConnectivityGraph();

    /// Get a path from this goal
    ROS_WARN("Planning from %s to %s", current_state_.c_str(), goal_constraint.goal_type.c_str());
    constraints = connectivity_graph_.getPlan(current_state_,goal_constraint.goal_type);

    ROS_DEBUG("Constraints: size = %i",(int)constraints.size());
    
    displayConstraints(constraints);

    std::vector<amigo_whole_body_controller::ArmTaskGoal>::iterator iter = constraints.begin();
    constraints.erase(iter);

    ROS_INFO("Constraints after erasing first: size = %i",(int)constraints.size());

    /// Update the current state
    // ToDo: make this more generic
    //current_state_ = goal_constraint.goal_type;
    //current_state_ = "reset";
    return true;
}

void PlannerTopological::displayConnectivityGraph()
{
    /// Initialize message
    visualization_msgs::MarkerArray marker_array;

    /// Get list of nodes
    std::vector<Node*> node_list = connectivity_graph_.getNodeList();

    /// Loop over list of nodes
    unsigned int id = 100; // ID for markers
    for (unsigned int i = 0; i < node_list.size(); i++)
    {
        /// Get all edges
        std::vector<Edge> edge_list = node_list[i]->getAdjNodeList();

        /// Loop over edges: create an array marker for every edge
        for (unsigned int j = 0; j < edge_list.size(); j++)
        {
            /// Get pointers to nodes
            Node* org_node = edge_list[j].getOrgNode();
            Node* dst_node = edge_list[j].getDstNode();

            /// Initialize marker
            visualization_msgs::Marker marker;
            marker.header.stamp = ros::Time::now();
            marker.header.frame_id = "/amigo/"+org_node->position_constraint_.header.frame_id;
            //marker.ns = "planner_topological";
            marker.id = id; ++id;
            marker.type = visualization_msgs::Marker::ARROW;
            marker.action = visualization_msgs::Marker::ADD;
            marker.color.a = 1.0;
            marker.color.r = 0.0;
            marker.color.g = 0.0;
            marker.color.b = 1.0;
            marker.scale.x = 0.01;
            marker.lifetime = ros::Duration(5.0);
            marker.points.push_back(org_node->position_constraint_.position);
            marker.points.push_back(dst_node->position_constraint_.position);

            /// Add marker to array
            marker_array.markers.push_back(marker);
        }
    }
    marker_array_pub_.publish(marker_array);
}

void PlannerTopological::displayConstraints(std::vector<amigo_whole_body_controller::ArmTaskGoal>& constraints) {
	
	/// Initialize message
    visualization_msgs::MarkerArray marker_array;
	
	for (unsigned int i = 0; i < constraints.size(); i++) {
		
		/// Nodes
		visualization_msgs::Marker marker;
		marker.header.stamp = ros::Time::now();
		marker.header.frame_id = "/amigo/"+constraints[i].position_constraint.header.frame_id;
		marker.id = i+1234;
		marker.type = 2;//sphere
		marker.scale.x = 0.05;
		marker.scale.y = 0.05;
		marker.scale.z = 0.05;
		marker.color.b = 1.0;
		marker.color.a = 1.0;
		marker.lifetime = ros::Duration(200.0);
		marker.pose.position.x = constraints[i].position_constraint.position.x;
		marker.pose.position.y = constraints[i].position_constraint.position.y;
		marker.pose.position.z = constraints[i].position_constraint.position.z;
		
		marker_array.markers.push_back(marker);
		
		/// Edges
		if (i > 0) {
			visualization_msgs::Marker edgemarker;
            edgemarker.header.stamp = ros::Time::now();
            edgemarker.header.frame_id = "/amigo/"+constraints[i].position_constraint.header.frame_id;
            //marker.ns = "planner_topological";
            edgemarker.id = i+234;
            edgemarker.type = visualization_msgs::Marker::ARROW;
            edgemarker.action = visualization_msgs::Marker::ADD;
            edgemarker.color.a = 1.0;
            edgemarker.color.r = 0.0;
            edgemarker.color.g = 0.0;
            edgemarker.color.b = 1.0;
            edgemarker.scale.x = 0.01;
            edgemarker.lifetime = ros::Duration(5.0);
            edgemarker.points.push_back(constraints[i-1].position_constraint.position);
            edgemarker.points.push_back(constraints[i].position_constraint.position);

            /// Add marker to array
            //marker_array.markers.push_back(edgemarker);
		}
		
	}
	
	marker_array_pub_.publish(marker_array);
}

void PlannerTopological::setCurrentState(const std::string &current_state) {
    current_state_ = current_state;
}
