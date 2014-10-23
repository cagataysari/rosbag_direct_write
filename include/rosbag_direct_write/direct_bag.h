/* Copyright 2014 Open Source Robotics Foundation, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ROSBAG_BAG_DIRECT_H
#define ROSBAG_BAG_DIRECT_H

#include <boost/shared_array.hpp>
#include <ros/header.h>
#include <ros/message_traits.h>
#include <ros/serialization.h>
#include <rosbag/constants.h>
#include <rosbag/exceptions.h>
#include <rosbag/structures.h>

namespace rosbag_direct_write
{

using boost::shared_ptr;

typedef std::vector<uint8_t> VectorBuffer;

class ROSBAG_DECL DirectFile
{
public:
  DirectFile(std::string filename);

  size_t get_offset() const;
  void seek(uint64_t pos) const;

  size_t write_buffer(VectorBuffer &buffer);
  size_t write_data(const uint8_t *start, size_t length);

  std::string get_filename() {return this->filename_;}

private:
  std::string filename_;
  FILE *file_pointer_;

};

class ROSBAG_DECL DirectBag
{
public:
  DirectBag(std::string filename);

  ~DirectBag();

  template<class T>
  void write(std::string const& topic,
             ros::Time const& time,
             shared_ptr<T const> const& msg,
             shared_ptr<ros::M_string> connection_header = 0)
  {
    write(topic, time, *msg, connection_header);
  }

  template<class T>
  void write(std::string const& topic,
             ros::Time const& time,
             shared_ptr<T> const& msg,
             shared_ptr<ros::M_string> connection_header = 0)
  {
    write(topic, time, *msg, connection_header);
  }

  template<class T>
  void write(std::string const& topic,
             ros::Time const& time,
             T const& msg,
             shared_ptr<ros::M_string> connection_header = 0);

  void close();
  bool is_open() const;

private:
  template<class T> shared_ptr<rosbag::ConnectionInfo>
  get_connection_info(std::string const &topic,
                      T const& msg,
                      shared_ptr<ros::M_string> connection_header);

  template<class T> void
  write_message_data_record(uint32_t conn_id,
                            ros::Time const& time,
                            T const& msg,
                            rosbag::ChunkInfo &chunk_info);


  std::string filename_;
  shared_ptr<DirectFile> file_;

  bool open_;

  size_t file_header_record_offset_;

  std::map<std::string, uint32_t> topic_connection_ids_;
  std::map<ros::M_string, uint32_t> header_connection_ids_;

  std::map<uint32_t, shared_ptr<rosbag::ConnectionInfo>> connections_;
  std::map<uint32_t, std::multiset<rosbag::IndexEntry> > connection_indexes_;
  std::vector<rosbag::ChunkInfo> chunk_infos_;

  VectorBuffer chunk_buffer_;

};

} // namespace rosbag_direct_write

#include "direct_bag_impl.h"

#endif