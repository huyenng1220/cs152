class Job < ActiveRecord::Base
	belongs_to :user
    has_many :votes, dependent: :destroy
	include PublicActivity::Model
	tracked

	def self.search(search)
			if search
				where(["company LIKE ?", "%#{search}%"])
			else
				all
			end
		end

end
