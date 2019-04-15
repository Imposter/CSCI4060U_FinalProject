/**
 * CSCI4060U: Massively Parallel Programming
 * Final Project: Parallel Jaccard Similarity
 * Date: April 15th, 2019
 * Author: Eyaz Rehman
 */

#pragma once

#include <pthread.h>
#include <cassert>

// Thread helper functions
pthread_t *thread_create(pthread_t *t, void (*routine)(void *), void *arg, bool joinable) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);
    const auto err = pthread_create(t, &attr, (void *(*)(void *))routine, arg);
    pthread_attr_destroy(&attr);
    if (err) {
        t = nullptr;
    }

    return t;
}

bool thread_join(pthread_t *t) {
    if (!t) return 0;
    void *status;
    const auto err = pthread_join(*t, &status);
    return err == 0;
}

bool thread_detach(pthread_t *t) {
    if (!t) return 0;
    const auto err = pthread_detach(*t);
    return err == 0;
}

// Mutex helper functions
pthread_mutex_t *mutex_create(pthread_mutex_t *m) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    const auto err = pthread_mutex_init(m, &attr);
    pthread_mutexattr_destroy(&attr);
    if (err) {
        m = nullptr;
    }

    return m;
}

bool mutex_destroy(pthread_mutex_t *m) {
    const auto err = pthread_mutex_destroy(m);
    return err == 0;
}

bool mutex_lock(pthread_mutex_t *m) {
    const auto err = pthread_mutex_lock(m);
    return err == 0;
}

bool mutex_trylock(pthread_mutex_t *m) {
    const auto err = pthread_mutex_trylock(m);
    return err == 0;
}

bool mutex_unlock(pthread_mutex_t *m) {
    const auto err = pthread_mutex_unlock(m);
    return err == 0;
}