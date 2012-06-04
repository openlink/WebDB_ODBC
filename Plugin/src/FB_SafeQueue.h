/**********************************************************\
#Original Author: Anthony Williams
#Imported and Adapted By: Richard Bateman
#Original Source: http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
# 
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2008
#\**********************************************************/

#ifndef H_FB_SafeQueue
#define H_FB_SafeQueue

#include "mthreads.h"
#include <queue>


    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  SafeQueue
    ///
    /// @brief  Basic thread-safe queue class
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    template<typename Data>
    class SafeQueue
    {
    private:
        std::queue<Data> the_queue;
        SpinLock mLock;

    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void SafeQueue::push(Data const& data)
        ///
        /// @brief  Pushes an object onto the end of the queue.
        ///
        /// @param  data    The data. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void push(Data const& data)
        {
            mLock.Enter();
            the_queue.push(data);
            mLock.Leave();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool SafeQueue::empty() const
        ///
        /// @brief  Queries if the queue is empty 
        ///
        /// @return true if the Queue is empty, false if not. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool empty() const
        {
            AutoLock lock(mLock);
            return the_queue.empty();
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn bool SafeQueue::try_pop(Data& popped_value)
        ///
        /// @brief  Try to pop a value off the front of the queue; if the queue is empty returns false
        ///
        /// @param [out] popped_value    The popped value. 
        ///
        /// @return true if a value is returned, false if the queue was empty
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        bool try_pop(Data& popped_value)
        {
            AutoLock lock(mLock);
            if(the_queue.empty())
            {
                return false;
            }
            
            popped_value=the_queue.front();
            the_queue.pop();
            return true;
        }

    };

#endif //H_FB_SafeQueue

