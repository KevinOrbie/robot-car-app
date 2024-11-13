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
    NodeTrajectory(std::deque<position_t> nodes={}, bool closed=true): nodes_(nodes), closed_(closed) {};

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

    position_t nextNode(position_t curr_pos) {
        /* Handle Exceptions. */
        if (nodes_.empty()) {
            return {};
        } 
        
        position_t target_node = nodes_[next_node_index_];
        double node_distance = (target_node - curr_pos).norm();

        if (node_distance < POSITION_TOLERANCE) {
            if (next_node_index_ < static_cast<int>(nodes_.size()) - 1) {
                next_node_index_++;
            } else if (closed_) {
                next_node_index_ = 0;
            }
        }
        
        return nodes_[next_node_index_];
    }

   private:
    std::deque<position_t> nodes_ = {};
    int next_node_index_ = 0;
    bool closed_ = true;
    const double POSITION_TOLERANCE = 0.01; // 1 cm
};

