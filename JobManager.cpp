#include "JobManager.h"

JobManager::JobManager(Region& region): region(region) { }

Job& JobManager::create_build_job(Pos site, Form form, uint16_t substance) {
	Job* job = new BuildJob(site, form, substance);
	add_job(job);
	return *job;
}

void JobManager::add_job(Job* job) {
	job->id = (int)jobs.size();
	jobs.emplace_back(job);
	job_type_map[job->type].push_back(job);
}

void JobManager::change_priority(Job& job, int priority) {
	if (priority < -5) priority = -5;
	if (priority > 5) priority = 5;
	job.priority = priority;
}

Job* JobManager::get_oldest(Filter& filter) const {
	if (filter.type == Job::Any) {
		for (auto iter = jobs.begin(); iter != jobs.end(); ++iter) {
			Job& job = **iter;
			if (job.priority >= filter.minimum_priority && (!filter.func || filter.func(job))) {
				return &job;
			}
		}
	} else {
		auto& jobs_of_type = job_type_map.at((int)filter.type);
		for (auto iter = jobs_of_type.begin(); iter != jobs_of_type.end(); ++iter) {
			Job& job = **iter;
			if (job.priority >= filter.minimum_priority && (!filter.func || filter.func(job))) {
				return &job;
			}
		}
	}
	return nullptr;
}

std::vector<Job*> JobManager::get_all(Filter& filter) const {
	std::vector<Job*> jobs;
	if (filter.type == Job::Any) {
		for (auto iter = jobs.begin(); iter != jobs.end(); ++iter) {
			Job& job = **iter;
			if (job.priority >= filter.minimum_priority && (!filter.func || filter.func(job))) {
				jobs.push_back(&job);
			}
		}
	} else {
		auto& jobs_of_type = job_type_map.at((int)filter.type);
		for (auto iter = jobs_of_type.begin(); iter != jobs_of_type.end(); ++iter) {
			Job& job = **iter;
			if (job.priority >= filter.minimum_priority && (!filter.func || filter.func(job))) {
				jobs.push_back(&job);
			}
		}
	}
	return jobs;
}

Job* JobManager::get_nearest(Filter& filter) const {
	return nullptr; // TODO
}

Filter::Filter() {
	minimum_priority = -5;
	type = Job::Any;
}
