class Profile < ActiveRecord::Base
	##Image Uploading
	  mount_uploader :avatar, AvatarUploader

		belongs_to :user
    validates_associated :user

	def self.search(search)

			if search
				where(["occupation LIKE ?", "%#{search}%"])
			else
				all
			end
		end
end
