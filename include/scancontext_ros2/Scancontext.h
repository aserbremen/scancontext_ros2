#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include <pcl/filters/voxel_grid.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>

#include "KDTreeVectorOfVectorsAdaptor.h"
#include "nanoflann.hpp"

#include "tictoc.h"
#include <rclcpp/rclcpp.hpp>

using namespace Eigen;
using namespace nanoflann;

using std::cout;
using std::endl;
using std::make_pair;

using std::atan2;
using std::cos;
using std::sin;

using SCPointType = pcl::PointXYZI; // using xyz only. but a user can exchange the original bin encoding function (i.e., max hegiht) to max
                                    // intensity (for detail, refer 20 ICRA Intensity Scan Context)
using KeyMat = std::vector<std::vector<float>>;
using InvKeyTree = KDTreeVectorOfVectorsAdaptor<KeyMat, float>;

// namespace SC2
// {

void coreImportTest(void);

// sc param-independent helper functions
float xy2theta(const float &_x, const float &_y);
MatrixXd circshift(MatrixXd &_mat, int _num_shift);
std::vector<float> eig2stdvec(MatrixXd _eigmat);

class SCManager {
public:
    SCManager() = default; // reserving data space (of std::vector) could be considered. but the descriptor is lightweight so don't care.
    SCManager(rclcpp::Node::SharedPtr _node);

    Eigen::MatrixXd makeScancontext(const pcl::PointCloud<SCPointType> &_scan_down);
    Eigen::MatrixXd makeRingkeyFromScancontext(Eigen::MatrixXd &_desc);
    Eigen::MatrixXd makeSectorkeyFromScancontext(Eigen::MatrixXd &_desc);

    int fastAlignUsingVkey(MatrixXd &_vkey1, MatrixXd &_vkey2);
    double distDirectSC(MatrixXd &_sc1, MatrixXd &_sc2);                           // "d" (eq 5) in the original paper (IROS 18)
    std::pair<double, int> distanceBtnScanContext(MatrixXd &_sc1, MatrixXd &_sc2); // "D" (eq 6) in the original paper (IROS 18)

    // User-side API
    void makeAndSaveScancontextAndKeys(const pcl::PointCloud<SCPointType> &_scan_down, const std::string &human_readable_id);
    std::pair<int, float> detectLoopClosureID(void); // int: nearest node index, float: relative yaw

public:
    // hyper parameters ()
    double LIDAR_HEIGHT = 2.0; // lidar height : add this for simply directly using lidar scan in the lidar local coord (not robot
                               // base coord) / if you use robot-coord-transformed lidar scans, just set this as 0.

    int PC_NUM_RING = 20;        // 20 in the original paper (IROS 18)
    int PC_NUM_SECTOR = 60;      // 60 in the original paper (IROS 18)
    double PC_MAX_RADIUS = 80.0; // 80 meter max in the original paper (IROS 18)
    double PC_UNIT_SECTORANGLE = 360.0 / double(PC_NUM_SECTOR);
    double PC_UNIT_RINGGAP = PC_MAX_RADIUS / double(PC_NUM_RING);

    // tree
    int NUM_EXCLUDE_RECENT = 50;       // simply just keyframe gap, but node position distance-based exclusion is ok.
    int NUM_CANDIDATES_FROM_TREE = 10; // 10 is enough. (refer the IROS 18 paper)

    // loop thres
    double SEARCH_RATIO =
        0.1; // for fast comparison, no Brute-force, but search 10 % is okay. // not was in the original conf paper, but improved ver.
    double SC_DIST_THRES = 0.13; // empirically 0.1-0.2 is fine (rare false-alarms) for 20x60 polar context (but for 0.15 <, DCS or
                                 // ICP fit score check (e.g., in LeGO-LOAM) should be required for robustness)
    //  double SC_DIST_THRES = 0.5; // 0.4-0.6 is good choice for using with robust kernel (e.g., Cauchy, DCS) + icp fitness threshold
    // / if not, recommend 0.1-0.15

    // config
    int TREE_MAKING_PERIOD_ = 50; // i.e., remaking tree frequency, to avoid non-mandatory every remaking, to save time cost / if you
                                  // want to find a very recent revisits use small value of it (it is enough fast ~ 5-50ms wrt N.).
    int tree_making_period_conter = 0;

    // data
    std::vector<double> polarcontexts_timestamp_; // optional.
    std::vector<Eigen::MatrixXd> polarcontexts_;
    std::vector<Eigen::MatrixXd> polarcontext_invkeys_;
    std::vector<Eigen::MatrixXd> polarcontext_vkeys_;
    std::vector<std::string> human_readable_ids_;

    KeyMat polarcontext_invkeys_mat_;
    KeyMat polarcontext_invkeys_to_search_;
    std::unique_ptr<InvKeyTree> polarcontext_tree_;

}; // SCManager

// } // namespace SC2
