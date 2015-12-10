/***************************************************************************A
 * 
 * Copyright (c) 2014 aishuyu.com, Inc. All Rights Reserved
 * 
 **************************************************************************/
 
 
 
/**
 * @file libev_thread_pool.h
 * @author aishuyu(asy5178@163.com)
 * @date 2014/11/07 15:32:07
 * @brief 
 *  
 **/




#ifndef  __LIBEV_THREAD_POOL_H_
#define  __LIBEV_THREAD_POOL_H_


//#include <pthread.h>
#include <errno.h>
#include <stdio.h>

#include "atomic.h"
#include "pthread_cond.h"

namespace libevrpc {

using namespace PUBLIC_UTIL;

struct Task {
    // callback funtion
    void *(*process)(void * arg);
    void *param;
    struct Task *prev;
    struct Task *next;
};

struct TaskList {
    int32_t task_num;
    struct Task *task_head;
};

/*
 * rpc call info item
 */
typedef struct RequestQueueItem RQ_ITEM;
struct RequestQueueItem {
    // callback funtion
    void *(*processor)(void * arg);
    void *param;
    struct RQ_ITEM *prev;
    struct RQ_ITEM *next;
};

/*
 * rpc call queue
 */
typedef struct RequestQueue RQ;
struct RequestQueue {
    RQ_ITEM* head;
    RQ_ITEM* tail;
    PUBLIC_UTIL::Mutex q_mutex;
};

/*
 * thread with libev info
 */
class LibevThreadPool;
typedef struct {
    pthread_t thread_id;
    struct ev_loop* epoller;
    struct ev_io libev_watcher;
    int32_t notify_receive_fd;
    int32_t notify_send_fd;
    RQ* new_request_queue;
    // point to the current ptr
    LibevThreadPool* lt_pool;
} LIBEV_THREAD;

class LibevThreadPool {
    public:
        // default ctor
        LibevThreadPool();

        LibevThreadPool(int nthread);

        virtual ~LibevThreadPool();

        bool Start();

        bool Wait();

        // nonblock call the processor and return shortly
        bool Processor(void *(*process) (void *arg), void *arg);

        bool DispatchRpcCall(void *(*process) (void *arg), void *arg);

    private:
        bool Initialize();

        bool LibevThreadInitialization();

        RQ_ITEM* RQNew();

        bool RQPush(RQ* req_queue, RQ_ITEM* req_item);

        static RQ_ITEM* RQPop(RQ* req_queue);

        static void *WorkerThread(void *arg);

        static void LibevProcessor(struct ev_loop *loop, struct ev_io *watcher, int revents);

        static void *LibevWorker(void *arg);

        bool Destroy();

    private:
        int32_t current_thread_;
        int32_t num_threads_;

        LIBEV_THREAD* libev_threads_;
        CQ*  cq_freelist_

        PUBLIC_UTIL::Mutex cq_freelist_mutex_;

        pthread_t *thread_ids_;

        int32_t nthread_num_;
        int32_t max_nthread_num_;
        static volatile ATOMIC_BOOL running_;

        PUBLIC_UTIL::Mutex task_mutex_;
        Condition cond_;
        TaskList task_list_;
};


} // end of namespace libevrpc

#endif  //__LIBEV_THREAD_POOL_H_

/* vim: set expandtab ts=4 sw=4 sts=4 tw=100: */
