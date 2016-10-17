class CreateProfiles < ActiveRecord::Migration
  def change
    create_table :profiles do |t|
      t.string :occupation
      t.text :education
      t.text :resume
      t.text :skill

      t.timestamps null: false
    end
  end
end
