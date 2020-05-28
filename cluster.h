/* -*- c++ -*-
 *
 * Copyright (c) 2020 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_CLUSTER_H
#define OLYMP_CLUSTER_H

#include <functional>
#include <vector>

/**
 * Filtering a sequence of T so that it comes out in clusters,
 * but doesn't come out later than necessary.
 *
 * Used to print photo entries with hh:mm:ss time stamps if they
 * are taken closely together.
 */
template <class T>
class Cluster {
public:
    explicit Cluster(std::function<bool(const T&, const T&)> near)
	: near{near}
    {}

    std::vector<T> add(const T&);
    std::vector<T> end() { return v; }

private:
    const std::function<bool(const T&, const T&)> near;
    std::vector<T> v;
};

template <class T>
std::vector<T> Cluster<T>::add(const T& t)
{
    std::vector<T> cluster;

    if (v.empty()) {
	v.push_back(t);
	return cluster;
    }

    const T& prev = v.back();

    if (near(prev, t)) {
	v.push_back(t);
	return cluster;
    }

    cluster.push_back(t);
    std::swap(cluster, v);
    return cluster;
}

#endif
