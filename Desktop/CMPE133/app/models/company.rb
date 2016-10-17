class Company < ActiveRecord::Base
  mount_uploader :avatar, AvatarUploader
end
