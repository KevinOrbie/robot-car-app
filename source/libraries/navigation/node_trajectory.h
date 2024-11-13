/**
 * @file node_trajectory.h
 * @author Kevin Orbie
 * 
 * @brief Defines a trajectory class.
 */

#pragma once

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <deque>

/* Custom C++ Libraries */
#include "common/pose.h"


/* =========================== Class =========================== */
class NodeTrajectory {
   public:
    NodeTrajectory(std::deque<position_t> nodes={}): nodes_(nodes) {};

    position_t closestNode(position_t curr_pos) {
        position_t closest_node;
        double closest_distance = -1;

        for(position_t node_pos: nodes_) {
            double node_distance = (node_pos - curr_pos).norm();
            if (closest_distance < node_distance) {
                closest_distance = node_distance;
                closest_node = node_pos;
            }
        }

        return closest_node;
    }

    position_t nextNode(position_t) {
        return {};
    }

   private:
    std::deque<position_t> nodes_ = {};
};

