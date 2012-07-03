/*
 * rgbd_evaluator_preprocessing.h
 *
 *  Created on: Jan 13, 2012
 *      Author: praktikum
 */
#ifndef RGBD_EVALUATOR_PREPROCESSING_H_
#define RGBD_EVALUATOR_PREPROCESSING_H_

#include <ros/ros.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <rosbag/message_instance.h>

#include <boost/foreach.hpp>

#include <message_filters/subscriber.h>
#include <message_filters/time_synchronizer.h>

#include <sensor_msgs/Image.h>
#include <geometry_msgs/TransformStamped.h>
#include <sensor_msgs/CameraInfo.h>

#include <LinearMath/btTransform.h>

#include <cv_bridge/cv_bridge.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <tf/tf.h>


namespace rgbd_evaluator
{

class RgbdEvaluatorPreprocessing
{
public:

  RgbdEvaluatorPreprocessing(std::string, bool reverse_order, int start_img );
  virtual ~RgbdEvaluatorPreprocessing();

  void readBag();
  void estimateHomographies();
  void recomputeImages();

  std::vector<cv::Point2f> mouse_kp_first_;
  std::vector<cv::Point2f> mouse_kp_current_;
  std::vector<cv::Point2f> opt_kp_last_;
  std::vector<cv::Point2f> mousePointsROI_;

  cv::Mat imageChooseROI_;
  cv::Mat keyPointImageOrigin_;
  cv::Mat keyPointImageCamX_;

  bool first_image_;
  bool finishedROI_;
  bool finishedKP_;

private:

  float getCamRotation( cv::Mat imgx, cv::Matx33f homography_final );

  cv::Matx33f calculateInitialHomography( btTransform transform_camx_to_original, btTransform transform_original );

  bool checkBoarderKP( cv::Mat image, float_t x_pos, float_t y_pos );

  int32_t calculateNCC(cv::Mat image_original, cv::Mat image_cam_x, cv::KeyPoint keypoint, cv::Point2f& keypointNCC, int i);

  void writeHomographyToFile(cv::Matx33f homography, uint32_t count);

  void writeIntrinsicMatToFile(cv::Matx33f K);

  void writeVectorToFile( std::vector<float> vec, std::string filename );

  void writeMaskPointsToFile( std::vector<cv::Point2f> maskPoints );

  void writeDepth( cv::Mat& depth_img_orig, std::string count_str );

  void markMissingDepthInfo( cv::Mat& rgb_image, cv::Mat& depth_image );

  void getKeypointsROI( cv::Mat& rgb_image, cv::Mat& depth_image );

  tf::StampedTransform calculateCoordinatesystem( cv::Mat& depth_img, std::vector<cv::Point2f> mouseKeypoints );

  void printMat( cv::Matx33f M );
  float getAspect( std::vector< cv::Point2f > pts );
  void splitFileName ( const std::string& str );
  void drawNumbers( cv::Mat& image, std::vector<cv::Point2f> keypoints );
  void drawKeypoints( cv::Mat& image, std::vector<cv::Point2f>& keypoints );

  static std::string int2str( uint32_t i );
  static void insertKeypoints( cv::Point2f keypoint, std::vector<cv::Point2f>& vec );

  static void imgMouseCallbackKP( int event, int x, int y, int flags, void* param );
  static void imgMouseCallbackROI( int event, int x, int y, int flags, void* param );

  void writeImage( const cv::Mat& imgx, const cv::Mat& img1, cv::Matx33f homography_final,
      std::string count_str  );

  bool readMatrix(const std::string & fileName, cv::Matx33f& K);

  std::string file_path_;
  std::string file_name_;
  std::string file_folder_;
  std::string file_created_folder_;

  bool reverse_order_;
  int start_img_;

  rosbag::Bag bag_;

  cv_bridge::CvImagePtr tmp_image_;

  cv::Matx33f K_;

  static const uint32_t BUFF_SIZE = 500;
  static const uint32_t MAX_CORRESPONDENCES_DIST_THRES = 10;
  static const uint32_t MIN_CORRESPONDENCES = 4;
  static const uint32_t MIN_FEATURE_NEIGHBOUR_DIST = 10;
  static const uint32_t MAX_FEATURE_NUMBER = 200;
  static const uint32_t SLIDING_WINDOW_SIZE = 40;
  static const uint32_t SEARCH_WINDOW_SIZE = SLIDING_WINDOW_SIZE+20;
  static const uint32_t KP_NEIGHBOURHOOD_WINDOW = 35;
  static const uint32_t KP_TEXT_PIXEL_OFFSET = 10;

  static const float_t  MIN_NCC_VAL = 0.98;

  struct ImageData
  {
    cv::Mat rgb_image;
    cv::Mat depth_image;
    cv::Matx33f homography;

    bool isComplete()
    {
      return rgb_image.rows > 0 && depth_image.rows > 0;
    }
  };

  std::vector< ImageData > image_store_;
};

}
#endif /* RGBD_EVALUATOR_PREPROCESSING_H_ */
