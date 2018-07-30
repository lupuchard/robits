#ifndef TOWN_JOBMANAGER_H
#define TOWN_JOBMANAGER_H

#include "Region/Region.h"
#include <deque>
#include <unordered_map>

class JobManager;

class Job {
public:
	enum Type {
		Any,
		Build
	};

	Job(Type type): type(type) { }

	inline Type get_type() { return type; }

private:
	friend JobManager;
	Type type;
	int priority = 0;
	int id;
};

struct BuildJob: public Job {
	BuildJob(Pos site, Form form, uint16_t substance):
		Job(Job::Build), site(site), substance(substance), form(form) { }

	Pos site;
	uint16_t substance;
	Form form;
};

struct Filter {
	Filter();
	Job::Type type;
	int minimum_priority;
	std::function<bool(Job&)> func;
};

class JobManager {
public:
	JobManager(Region& region);

	Job& create_build_job(Pos site, Form form, uint16_t substance);

	void change_priority(Job& job, int priority);

	Job* get_oldest(Filter& filter) const;
	Job* get_nearest(Filter& filter) const;
	std::vector<Job*> get_all(Filter& filter) const;

private:
	void add_job(Job* job);

	std::deque<std::unique_ptr<Job>> jobs;
	std::unordered_map<int, std::deque<Job*>> job_type_map;

	Region& region;
};


#endif //TOWN_JOBMANAGER_H
