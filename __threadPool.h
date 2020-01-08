// Пытаемся разобраться в чужом Thread Pool
// https://habrahabr.ru/post/188234/
//
// and another one to see to: https://github.com/progschj/ThreadPool
// and about c++11 threads:   https://habrahabr.ru/post/182610/
// -----------------------------------------------------------------------------------

#pragma once
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

typedef std::function<void()> fn_type;



// Класс одного потока
class Worker {    

 public:
	Worker()
		:enabled(true),
        fqueue(),
        thread(&Worker::thread_fn, this)
	{}

   ~Worker() {
		enabled = false;
		cv.notify_one();	
		thread.join();
	}

	void appendFn(fn_type fn) {

		std::unique_lock<std::mutex> locker(mutex);
		fqueue.push(fn);			
		cv.notify_one();
	}

	size_t getTaskCount() { 

		std::unique_lock<std::mutex> locker(mutex);
		return fqueue.size();		
	}

	bool isEmpty() {

		std::unique_lock<std::mutex> locker(mutex);
		return fqueue.empty();	
	}

 private:
    void thread_fn()
    {
	    while (enabled)
	    {
		    std::unique_lock<std::mutex> locker(mutex);

		    // Ожидаем уведомления, и убедимся что это не ложное пробуждение
		    // Поток должен проснуться если очередь не пустая либо он выключен
		    cv.wait(locker, [&](){ return !fqueue.empty() || !enabled; });				

		    while(!fqueue.empty())
		    {
			    fn_type fn = fqueue.front();
			    // Разблокируем мютекс перед вызовом функтора
			    locker.unlock();
			    fn();
			    // Возвращаем блокировку снова перед вызовом fqueue.empty() 
			    locker.lock();
			    fqueue.pop();
		    }				
	    }
    }

 private:
	bool				     enabled;
	std::condition_variable	 cv;
	std::queue<fn_type>		 fqueue;
	std::mutex				 mutex;
	std::thread				 thread;	
};

typedef std::shared_ptr<Worker> worker_ptr;



// Класс пула потоков
class ThreadPool {

 public:
	ThreadPool(size_t threads = 1) {

		if ( threads < 1 )
			threads = 1;

		for (size_t i = 0; i < threads; i++) {
			worker_ptr pWorker(new Worker);
			_workers.push_back(pWorker);
		}
	}
   ~ThreadPool() {}

    // Вызвать любой внешний метод с любым числом параметров
    template<class _FN, class... _ARGS>
    void runAsync(_FN _fn, _ARGS... _args) {
	    getFreeWorker()->appendFn(std::bind(_fn, _args...));
    }

    // дождаться, пока все потоки отработают все свои задачи
    void waitForAll() {

        bool result;

        do {
            result = false;

	        for ( auto &it : _workers )
                if ( !it->isEmpty() ) {
                    result = true;
                    break;
                }
        
        } while (result);

        return;
    }

 private:
    // Функция находит ближайший свободный поток либо поток у которого меньше всего заданий и возвращает его. 
    // Таким образом задания будут распределены между потоками равномерно но это не гарантирует оптимальное распределение нагрузок. 
    worker_ptr ThreadPool::getFreeWorker() {

	    worker_ptr pWorker;
	    size_t minTasks = UINT32_MAX;				

	    for (auto &it : _workers) {

		    if (it->isEmpty())
			    return it;
		    else
                if ( it->getTaskCount() < minTasks ) {
			        minTasks = it->getTaskCount();
			        pWorker = it;
		        }
	    }

	    return pWorker;
    }

 private:
	std::vector<worker_ptr>  _workers; 
};

#endif
