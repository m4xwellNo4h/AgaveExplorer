/*********************************************************************************
**
** Copyright (c) 2017 The University of Notre Dame
** Copyright (c) 2017 The Regents of the University of California
**
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice, this
** list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above copyright notice, this
** list of conditions and the following disclaimer in the documentation and/or other
** materials provided with the distribution.
**
** 3. Neither the name of the copyright holder nor the names of its contributors may
** be used to endorse or promote products derived from this software without specific
** prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
** SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
** BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
** IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
***********************************************************************************/

// Contributors:
// Written by Peter Sempolinski, for the Natural Hazard Modeling Laboratory, director: Ahsan Kareem, at Notre Dame

#include "joboperator.h"

#include "remotefiletree.h"
#include "remotejoblister.h"
#include "../AgaveClientInterface/remotedatainterface.h"
#include "../AgaveClientInterface/remotejobdata.h"

JobOperator::JobOperator(RemoteDataInterface * newDataLink, QObject * parent) : QObject((QObject *)parent)
{
    dataLink = newDataLink;
}

void JobOperator::linkToJobLister(RemoteJobLister * newLister)
{
    newLister->setModel(&theJobList);
}

void JobOperator::refreshRunningJobList(RequestState replyState, QList<RemoteJobData> * theData)
{
    if (replyState != RequestState::GOOD)
    {
        //TODO: some error here
        return;
    }

    //TODO: Subsequent versions should update rather than wholesale re-write

    rawData.clear(); //TODO: make sure no memory leak here
    for (auto itr = theData->begin(); itr != theData->end(); itr++)
    {
        RemoteJobData * newItem = new RemoteJobData();
        (*newItem) = (*itr);
        rawData.append(newItem);
    }

    theJobList.clear(); //TODO: make sure no memory leak here
    theJobList.setHorizontalHeaderLabels({"Task Name", "State", "Agave App", "Time Created", "Agave ID"});

    for (auto itr = rawData.begin(); itr != rawData.end(); itr++)
    {
        QList<QStandardItem *> newRow;
        newRow.append(new QStandardItem((*itr)->getName()));
        newRow.append(new QStandardItem((*itr)->getState()));
        newRow.append(new QStandardItem((*itr)->getApp()));
        newRow.append(new QStandardItem((*itr)->getTimeCreated().toString()));
        newRow.append(new QStandardItem((*itr)->getID()));
        theJobList.appendRow(newRow);
    }
}

void JobOperator::demandJobDataRefresh()
{
    RemoteDataReply * listReply = dataLink->getListOfJobs();
    QObject::connect(listReply, SIGNAL(haveJobList(RequestState,QList<RemoteJobData>*)),
                     this, SLOT(refreshRunningJobList(RequestState,QList<RemoteJobData>*)));
}
