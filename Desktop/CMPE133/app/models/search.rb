class Search < ActiveRecord::Base


		def find_jobs
			
			jobs = Job.all

			jobs = jobs.where(["company LIKE?", company ]) if company.present?
			jobs = jobs.where(["title LIKE?", title  ]) if title.present?
			jobs = jobs.where(["description LIKE?", description ]) if description.present?

			return jobs
		end
end
