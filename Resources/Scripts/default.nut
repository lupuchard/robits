
function find_job(robot) {
	job = Jobs.get_next();
	if (job) {
		robot.claim_job(job);
	}
}

function do_job(robot) {
	job = robot.get_job();
	if (job.type == BUILD) {
		robot.go_to(job).then(@() {
			robot.build(job).then(@() {
				robot.complete_job();
			}).or_else(@() {
				robot.abandon_job();
				find_job(robot);
			});
		}).or_else(@() {
			robot.abandon_job();
			find_job(robot);
		});
	}
}

function OnJobCreated(robot, job) {
	if (!robot.get_job()) {
		robot.claim_job(job);
		doJob(robot);
	}
}
