class HomeController < ApplicationController
   before_filter :authenticate_user!

  def index
    @user = current_user
    @post = Post.new
    @jobs = Job.order('created_at DESC')
    @activities = PublicActivity::Activity.order('created_at DESC')
  end
end
