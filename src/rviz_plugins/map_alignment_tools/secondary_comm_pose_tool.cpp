#include <tf/transform_listener.h>

#include <geometry_msgs/PoseWithCovarianceStamped.h>

#include <rviz/display_context.h>
#include <rviz/properties/string_property.h>

#include "secondary_comm_pose_tool.h"


namespace rviz
{

SecondaryMapCommPoseTool::SecondaryMapCommPoseTool()
{
  shortcut_key_ = 'S';

  topic_property_ = new StringProperty( "Topic", "/map_aligner/comm_pose_sec",
                                        "The topic on which to publish secondary-map common pose pose estimate",
                                        getPropertyContainer(), SLOT( updateTopic() ), this );
}

void SecondaryMapCommPoseTool::onInitialize()
{
  PoseTool::onInitialize();
  setName( "Secondary-Map Common Pose" );
  updateTopic();
}

void SecondaryMapCommPoseTool::updateTopic()
{
  pub_ = nh_.advertise<geometry_msgs::PoseWithCovarianceStamped>( topic_property_->getStdString(), 1 );
}

void SecondaryMapCommPoseTool::onPoseSet(double x, double y, double theta)
{
  std::string fixed_frame = context_->getFixedFrame().toStdString();
  geometry_msgs::PoseWithCovarianceStamped pose;
  pose.header.frame_id = fixed_frame;
  pose.pose.pose.position.x = x;
  pose.pose.pose.position.y = y;

  tf::Quaternion quat;
  quat.setRPY(0.0, 0.0, theta);
  tf::quaternionTFToMsg(quat,
                        pose.pose.pose.orientation);
  pose.pose.covariance[6*0+0] = 0.5 * 0.5;
  pose.pose.covariance[6*1+1] = 0.5 * 0.5;
  pose.pose.covariance[6*5+5] = M_PI/12.0 * M_PI/12.0;
  ROS_INFO("Setting pose: %.3f %.3f %.3f [frame=%s]", x, y, theta, fixed_frame.c_str());
  pub_.publish(pose);
}

} // end namespace rviz

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS( rviz::SecondaryMapCommPoseTool, rviz::Tool )