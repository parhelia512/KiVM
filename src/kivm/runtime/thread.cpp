//
// Created by kiva on 2018/3/25.
//

#include <kivm/method.h>
#include <kivm/runtime/thread.h>
#include <kivm/runtime_config.h>
#include <kivm/bytecode/interpreter.h>

namespace kivm {
    Thread::Thread(Method *method, const std::list<oop> &args)
            : _frames(RuntimeConfig::get().threadInitialStackSize),
              _method(method), _args(args),
              _java_thread_object(nullptr), _native_thread(nullptr),
              _pc(nullptr), _state(ThreadState::RUNNING) {
    }

    void Thread::create(instanceOop java_thread) {
        this->_java_thread_object = java_thread;
        this->_native_thread = new std::thread([this] {
            if (this->should_record_in_thread_table()) {
                D("should_record_in_thread_table == true, recording.");
                Threads::add(this);
            }
            this->start();
        });
        this->thread_lunched();
    }

    void Thread::thread_lunched() {
        // Do nothing.
    }

    bool Thread::should_record_in_thread_table() {
        return true;
    }

    Thread::~Thread() = default;


    JavaMainThread::JavaMainThread()
            : JavaThread(nullptr, {}) {
    }

    void JavaMainThread::start() {
        // Initialize Java Virtual Machine
        Threads::initializeJVM();

        // TODO: find main(String[]) method and build arg list
        // OK, call main() with args
        // this->_method = main_method;
        // this->_args = main_args;
        // Run method manually, we cannot use JavaThread::run()
        // because it is designed for app threads.
    }

    void JavaMainThread::thread_lunched() {
        // Start the first app thread to run main(String[])
        this->_native_thread->join();

        // Then, let's wait for all app threads to finish
        for (;;) {
            int threads = Threads::get_app_thread_count_locked();
            assert(threads >= 0);

            if (threads == 0) {
                break;
            }

            sched_yield();
        }
    }

    bool JavaMainThread::should_record_in_thread_table() {
        return false;
    }

    JavaThread::JavaThread(Method *method, const std::list<oop> &args)
            : Thread(method, args) {
    }

    void JavaThread::start() {
        // No other threads will join this thread.
        // So it is OK to detach()
        this->_native_thread->detach();

        // A thread must start with an empty frame
        assert(_frames._size == 0);

        // Only one argument(this) in java.lang.Thread#run()
        assert(_args.size() == 1);

        run_method(_method, _args);
        this->set_state(ThreadState::DIED);

        if (this->should_record_in_thread_table()) {
            Threads::dec_app_thread_count_locked();
        }
    }

    oop JavaThread::run_method(Method *method, const std::list<oop> &args) {
        Frame method_frame(method->get_max_locals(), method->get_max_stack());
        method_frame._method = method;
        method_frame._return_pc = this->_pc;
        method_frame._is_native_frame = method->is_native();

        this->_frames.push(&method_frame);
        oop result = ByteCodeInterpreter::interp(this);
        this->_frames.pop();

        this->_pc = method_frame._return_pc;
        return result;
    }

    void Threads::initializeJVM() {
        // TODO: put initialization logic here.
    }

    void Threads::add(Thread *java_thread) {
        inc_app_thread_count_locked();
    }
}
