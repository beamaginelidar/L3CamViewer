/*  Copyright (c) 2023, Beamagine
 *
    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:

        - Redistributions of source code must retain the above copyright notice,
          this list of conditions and the following disclaimer.
        - Redistributions in binary form must reproduce the above copyright notice,
          this list of conditions and the following disclaimer in the documentation and/or
          other materials provided with the distribution.
        - Neither the name of copyright holders nor the names of its contributors may be
          used to endorse or promote products derived from this software without specific
          prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
    COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
    EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifndef POINTCLOUDVIEWERCONTROLLERMESSAGES
#define POINTCLOUDVIEWERCONTROLLERMESSAGES
#include <QEvent>
#include <QString>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/file_io.h>


class pclPointCloudViewerControllerShowUdpPointCloud : public QEvent{
  public:
    pclPointCloudViewerControllerShowUdpPointCloud(int32_t *pointcloud, int32_t buff_size) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_point_cloud = (int32_t*)malloc((buff_size*sizeof(int32_t)*5)+1);
        memcpy(m_point_cloud, pointcloud, (sizeof(int32_t)*buff_size*5)+1);
        m_buff_size = buff_size;
    }
    static const QEvent::Type TYPE;
    int32_t *getPointCloud(){return m_point_cloud;}
    int32_t getBufferSize(){return m_buff_size;}
    void releaseMemory(){free(m_point_cloud);}
private:
    int32_t m_buff_size;
    int32_t *m_point_cloud;
};

class pclPointCloudViewerControllerPointSelectedNotification : public QEvent{
public:
    pclPointCloudViewerControllerPointSelectedNotification(QString point_selected) : QEvent((QEvent::Type)(QEvent::registerEventType())){
        m_point_selected = point_selected;
    }
    static const QEvent::Type TYPE;
    QString getPointSelected(){return m_point_selected;}
private:
    QString m_point_selected;
};

#endif // POINTCLOUDVIEWERCONTROLLERMESSAGES
