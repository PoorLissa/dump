// -----------------------------------------------------------------------------------------------
// Thread Pool:												https://habrahabr.ru/post/188234/
//
// And another one to see to:								https://github.com/progschj/ThreadPool
// And also about c++11 threads:							https://habrahabr.ru/post/182610/
// -----------------------------------------------------------------------------------------------

/*
	// Usage example:
	for (size_t listNo = 0; listNo < VEC.size(); listNo++)
		thPool->runAsync(&Game::threadMonsterMove, this, listNo, playerPosX, playerPosY);

	thPool->waitForAll();   // Wait untill all the threads finish their tasks
*/

// -----------------------------------------------------------------------------------------------

#pragma once
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <functional>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

// -----------------------------------------------------------------------------------------------

#define DEBUG_MSG_ON
#undef DEBUG_MSG_ON				// Comment this line to enable debug messages

#if defined DEBUG_MSG_ON
	extern std::mutex gl_Console_Mutex;
#endif

// -----------------------------------------------------------------------------------------------

// Single Worker Thread Class
class Worker {

public:
	struct workerTask;

	#define setLock(x, lockName) std::unique_lock<std::mutex> lockName(x);

	typedef std::function<void()>		fn_type;
	typedef std::shared_ptr<workerTask> taskPtr;

	// Helper structure. Holds the function to perform and other related data (such as complexity)
	struct workerTask {
		workerTask(const fn_type f, const size_t c) : func(f), complexity(c) { ; }
		fn_type func;
		size_t  complexity;
	};

	// ----------------------------------------------------------------------------------

	public:

		Worker() : threadEnabled(true), threadPersistent(false), fqueue(), thread(&Worker::thread_fn, this), complexity(0u)
		{
			#if defined DEBUG_MSG_ON
				setLock(gl_Console_Mutex, console_lock);
					std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Constructor()" << std::endl;
					console_lock.unlock();
			#endif
		}

	   ~Worker()
		{
			#if defined DEBUG_MSG_ON
				setLock(gl_Console_Mutex, console_lock);
					std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: ~Destructor()" << std::endl;
					console_lock.unlock();
			#endif

			threadEnabled    = false;
			threadPersistent = false;
			cndVar.notify_one();
			thread.join();
		}

		// Assigns new task to the worker thread
		void appendTask(fn_type task_func, size_t task_complexity = 1)
		{
			taskPtr task = getNewTask(task_func, task_complexity);

			setLock(mutex, lck);
				complexity += task_complexity;
				fqueue.push(task);
				cndVar.notify_one();

			#if defined DEBUG_MSG_ON
				setLock(gl_Console_Mutex, console_lock);
					std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Task assigned; Complexity Increased (" << complexity << ")" << std::endl;

					if( threadPersistent )
						std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: thread type changed -- Generic -> Persistent" << std::endl;
			#endif
		}

		void appendTask(taskPtr task)
		{
			setLock(mutex, lck);
				complexity += task->complexity;
				fqueue.push(task);
				cndVar.notify_one();

			#if defined DEBUG_MSG_ON
				setLock(gl_Console_Mutex, console_lock);
					std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Task assigned; Complexity Increased (" << complexity << ")" << std::endl;

					if (threadPersistent)
						std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: thread type changed -- Generic -> Persistent" << std::endl;
			#endif
		}

		// Checks is the pool waits for this worker to finish
		bool isPersistent() const
		{
			return threadPersistent;
		}

		// Sets Waitable property
		void makePersistent()
		{
			threadPersistent = true;
		}

		// Returns the number of tasks assigned to this worker
		size_t getTaskCount()
		{ 
			setLock(mutex, lck);
				return fqueue.size();
		}

		// Checks is the worker queue is empty
		bool isQueueEmpty()
		{
			setLock(mutex, lck);
				return fqueue.empty();
		}

		// Returns overall complexity of the worker
		size_t getWorkerComplexity()
		{
			setLock(mutex, lck);
				return complexity;
		}

		// Move all existing tasks (except for the 1st one) from the queue to outer container
		void giveUpTasks(std::vector<Worker::taskPtr> &vec)
		{
			if( !threadPersistent )
			{
				setLock(mutex, lck);

					if (fqueue.size() > 1u)
					{
						taskPtr first = fqueue.front();				// store the 1st item
						fqueue.pop();

						while (!fqueue.empty())						// move all the others
						{
							taskPtr ptr = fqueue.front();

							fqueue.pop();
							complexity -= ptr->complexity;
							vec.push_back(ptr);
						}

						fqueue.push(first);							// restore the 1st item
					}
			}

			return;
		}

	private:

		// Create and return new workerTask wrapped in shared_ptr
		taskPtr getNewTask(const fn_type &func, const size_t &complexity)
		{
			return taskPtr(new workerTask({ func, complexity } ));
		}

		// Endless loop function executed in this thread
		void thread_fn()
		{
			while( threadEnabled )
			{
				setLock(mutex, threadLock);
				{
					#if defined DEBUG_MSG_ON
						setLock(gl_Console_Mutex, console_lock);
							std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Thread goes to sleep" << std::endl;
							console_lock.unlock();
					#endif

					// Block the current thread until the condition variable is woken up.
					// Make sure the wake up event is not false: the thread must only be unblocked if:
					// a) the queue is not empty, OR
					// b) the thread has been disabled
					cndVar.wait(threadLock, [&]()
												{ 
													return !fqueue.empty() || !threadEnabled;
												}
					);

					#if defined DEBUG_MSG_ON
						console_lock.lock();
							std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Thread wakes up" << std::endl;
							console_lock.unlock();
					#endif

					// When the thread has woken up:
					while( !fqueue.empty() )
					{
						taskPtr task = fqueue.front();

						threadLock.unlock();				// Unlock the mutex before the Task Func is called
						{

							task->func();					// Call our target function

						}
						threadLock.lock();					// Lock the mutex again before calling fqueue.empty()

						complexity -= task->complexity;

						#if defined DEBUG_MSG_ON
							console_lock.lock();
								std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: Task finished; Complexity Decreased (" << complexity << ")" << std::endl;
								console_lock.unlock();
						#endif

						// Persistent threads automatically become generic as they finish the task assigned
						if (threadPersistent)
						{
							threadPersistent = false;

							#if defined DEBUG_MSG_ON
								console_lock.lock();
									std::cout << " -- DEBUG_MSG: thread[" << thread.get_id() << "] says: thread type changed -- Persistent -> Generic" << std::endl;
									console_lock.unlock();
							#endif
						}

						fqueue.pop();
					}
				}
			}
		}

	private:

		bool				     threadEnabled;			// the thread executes while this is True
		bool				     threadPersistent;		// the thread designed to be long-term and must be assessed in waitForAll(ThreadPool::PERSISTENT)
		std::condition_variable	 cndVar;				// condition variable used to wake up/put the thread to sleep
		std::queue<taskPtr>		 fqueue;				// thread task queue
		std::mutex				 mutex;					// thread mutex
		std::thread				 thread;				// the thread
		size_t					 complexity;			// thread overall complexity
};

// -----------------------------------------------------------------------------------------------

// Thread Pool Class
class ThreadPool {

	typedef std::shared_ptr<Worker> worker_ptr;

	public:

		enum ThreadType { ALL, GENERIC, PERSISTENT };

		struct PoolState {
			size_t	querySize;
			size_t	complexity;
			bool	isPersistent;
		};

	public:

		ThreadPool(size_t threads = 0)
		{
			if( !threads )
				threads = std::thread::hardware_concurrency();

			// Create worker threads and put them into vector
			for(size_t i = 0; i < threads; i++)
			{
				vec_workers.push_back( worker_ptr(new Worker) );
			}
		}

	   ~ThreadPool()
		{
		   ;
		}

		// Execute any external function with any number of parameters in a thread
		template<class _FN, class... _ARGS>
		void runAsync(_FN _fn, _ARGS... _args)
		{
			getWorker()->appendTask(std::bind(_fn, _args...));
		}

		// Execute any external function with any number of parameters in a thread (with custom Complexity parameter)
		template<class _FN, class... _ARGS>
		void runAsync_Complexity(size_t N, _FN _fn, _ARGS... _args)
		{
			getWorker()->appendTask(std::bind(_fn, _args...), N);
		}

		// Execute any external function with any number of parameters in a thread
		// The thread this task is assigned to will not be 
		template<class _FN, class... _ARGS>
		bool runAsync_Persistent(_FN _fn, _ARGS... _args)
		{
			worker_ptr w = getFreeWorker();

			if( w )
			{
				w->makePersistent();
				w->appendTask(std::bind(_fn, _args...));
			}

			return w != nullptr;
		}

		// Wait until all (specific) threads finish their tasks
		void waitForAll(ThreadType type = ThreadType::GENERIC)
		{
			bool (*predFunc)(const worker_ptr&);
			bool doWait(false);

			switch (type)
			{
				case ThreadType::GENERIC:
					predFunc = [](const worker_ptr& p) { return !p->isPersistent() && !p->isQueueEmpty(); };	// Wait for non-persistent threads only
					break;

				case ThreadType::PERSISTENT:
					predFunc = [](const worker_ptr& p) { return p->isPersistent() && !p->isQueueEmpty(); };		// Wait for persistent threads only
					break;

				default:
					predFunc = [](const worker_ptr& p) { return !p->isQueueEmpty(); };							// Wait for all threads
					break;
			}

			do {

				doWait = false;

				for (auto& it : vec_workers)
				{
					if (predFunc(it))
					{
						doWait = true;
						break;
					}
				}

			} while (doWait);

			return;
		}

		// Gets info on current state of threads
		void getThreadPoolState(std::vector<PoolState> &vec)
		{
			vec.clear();

			for (auto& it : vec_workers)
				vec.push_back(PoolState{ it->getTaskCount(), it->getWorkerComplexity(), it->isPersistent() });

			return;
		}

		// Redistribute existing tasks between all non-persistent workers
		void redistributeLoad()
		{
			std::vector<Worker::taskPtr> vec;

			for (auto& it : vec_workers)
				it->giveUpTasks(vec);

			for (auto& it : vec)
				getWorker()->appendTask(it);

			return;
		}

	private:

		// Finds and returns the first FREE worker thread
		// Returns nullptr in case no free workers found
		worker_ptr getFreeWorker()
		{
			for( auto& it : vec_workers )
				if( it->getWorkerComplexity() == 0u )
					return it;

			return nullptr;
		}

		// Finds and returns the first free worker thread (OR the thread with the MIN Complexity)
		// The worker must NOT be Persistent!
		// The worker then may call:
		// a) runAsync				-- The simplest case: Complexity equals to the number of tasks in the thread's queue. The tasks are distributed evenly between the workers, but this does not guarantee the optimal distribution of load.
		// b) runAsync_Complexity	-- The user must provide custom complexity of the task. Overall complexity of the worker is determined as a total of all tasks' complexities.
		worker_ptr getWorker()
		{
			worker_ptr pWorker;
			size_t min = SIZE_MAX, workerComplexity(0);

			for( auto& it : vec_workers )
			{
				if( !it->isPersistent() )
				{
					workerComplexity = it->getWorkerComplexity();

					if (workerComplexity == 0u)
					{
						pWorker = it;					// Free worker is found
						break;
					}
					else
					{
						if (workerComplexity < min)
						{
							min = workerComplexity;		// Worker with MIN complexity is found
							pWorker = it;
						}
					}
				}
			}

			return pWorker;
		}

	private:

		std::vector<worker_ptr> vec_workers;		// Vector of workers
};

// -----------------------------------------------------------------------------------------------

#endif
