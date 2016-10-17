json.array!(@profiles) do |profile|
  json.extract! profile, :id, :occupation, :education, :resume, :skill
  json.url profile_url(profile, format: :json)
end
