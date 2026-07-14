#pragma once

#include <set>
#include <unordered_map>

class TabulaPage : public wxPanel
{
public:
	TabulaPage(wxWindow* parent, sqlite3* db);
	~TabulaPage();


private:

	void generate_countries_and_regions();
	wxArrayString getCountryStrings();
	wxArrayString getBuildingSizeStrings();
	wxArrayString getBuildingTypeStrings();
	wxArrayString getElementTypeStrings();
	wxArrayString getBuildingStrings();
	wxArrayString getEnergySystemTableStrings();
	wxArrayString getEnergySystemsStrings();

	void BindEvent();

	void getTypologyBuildingTypeNum();
	void typology_Reset(wxCommandEvent& evt);

	void showDB_buildingType();
	void updateDB_buildingType();

	void showDB_building();
	void updateDB_building();

	void showDB_construction();
	void updateDB_construction();

	void showDB_heating_generator();
	void updateDB_heating_generator();

	void showDB_heating_storage();
	void updateDB_heating_storage();

	void showDB_heating_distribution();
	void updateDB_heating_distribution();

	void showDB_heating_aux();
	void updateDB_heating_aux();

	void showDB_water_heating_generator();
	void updateDB_water_heating_generator();

	void showDB_water_heating_storage();
	void updateDB_water_heating_storage();

	void showDB_water_heating_distribution();
	void updateDB_water_heating_distribution();

	void showDB_water_heating_aux();
	void updateDB_water_heating_aux();

	void showDB_heating_system();
	void updateDB_heating_system(const std::string& code = "");

	void showDB_water_heating_system();
	void updateDB_water_heating_system(const std::string& code = "");

	void updateDB_energySystem();
	void updateDB_energySystemComponent();

	void showDB_calculationSet();
	void updateDB_calculationSet(const std::string& code = "");

	void typology_selectCountry(wxCommandEvent& evt);
	void typology_selectBuildingSize(wxCommandEvent& evt);
	void typology_selectBuildingType(wxCommandEvent& evt);
	void typology_selectBuilding(wxCommandEvent& evt);
	void typology_selectElementType(wxCommandEvent& evt);
	void typology_enterYear(wxCommandEvent& evt);

	void typology_doubleClick_buildingType(wxCommandEvent& evt);
	void typology_doubleClick_building(wxCommandEvent& evt);

	void typology_copyUV(wxCommandEvent& evt);
	void typology_searchHeatingSystem(wxCommandEvent& evt);
	void typology_searchWaterHeatingSystem(wxCommandEvent& evt);

	void searchHeatingSystem();
	void searchWaterHeatingSystem();

	void energySystem_selectComponents(wxCommandEvent& evt);
	void energySystem_selectCountry(wxCommandEvent& evt);
	void energySystem_enterYear(wxCommandEvent& evt);
	void energySystem_selectBuildingSize(wxCommandEvent& evt);
	void energySystem_filterSystems(wxCommandEvent& evt);
	void energySystem_showAllSystems(wxCommandEvent& evt);

	void energySystem_selectSystem(wxCommandEvent& evt);
	void energySystem_doubleClick_systems(wxDataViewEvent& evt);	

	void calculationBuildingSetSearch(wxCommandEvent& evt);
	void onPressCopyCalculationUV(wxCommandEvent& evt);
	void selectCalculationSetBuilding(wxCommandEvent& evt);
	void calculateAverageUVofrefurbishedBuilding(wxDataViewItem& item);
	void copyCalculatedUVtoClipbrd();

	void calculateAverageOfBuilding();

	void energySystem_showComponentDetails(const std::string& type, const std::string& code);

	void energySystem_Generator1(wxCommandEvent& evt);
	void energySystem_Generator2(wxCommandEvent& evt);
	void energySystem_Generator3(wxCommandEvent& evt);
	void energySystem_Storage(wxCommandEvent& evt);
	void energySystem_Distribution(wxCommandEvent& evt);
	void energySystem_Auxiliary(wxCommandEvent& evt);

private:
	sqlite3* m_db;

	int m_typology_building_type_num;

	wxAuiNotebook* m_notebook;

	wxPanel* m_typology_page;
	wxPanel* m_energySystem_page;
	wxPanel* m_calculation_page;

	wxComboBox* m_typology_country_combobox;
	wxTextCtrl* m_typology_year_textctrl;
	wxComboBox* m_typology_buildingSizeClass_combobox;
	wxComboBox* m_typology_buildingType_combobox;
	wxComboBox* m_typology_building_combobox;
	wxComboBox* m_typology_elementType_combobox;
	wxButton* m_typology_reset_button;

	wxButton* m_typology_searchHeat_button;
	wxButton* m_typology_searchWaterHeat_button;

	wxTextCtrl* m_typology_average_buildingCode_textctrl;
	wxTextCtrl* m_typology_average_roof_uv_textctrl;
	wxTextCtrl* m_typology_average_wall_uv_textctrl;
	wxTextCtrl* m_typology_average_floor_uv_textctrl;
	wxTextCtrl* m_typology_average_window_uv_textctrl;
	wxTextCtrl* m_typology_average_door_uv_textctrl;
	wxTextCtrl* m_typology_average_window_gv_textctrl;
	wxStaticText* m_typology_search_results_text;
	wxButton* m_typology_average_copyUV_button;

	wxDataViewListCtrl* m_typology_buildingType_table;
	wxDataViewListCtrl* m_typology_building_table;
	wxDataViewListCtrl* m_typology_construction_table;

	wxComboBox* m_energySystem_component_combobox;
	wxTextCtrl* m_energySystem_year_textctrl;
	wxComboBox* m_energySystem_country_combobox;
	wxComboBox* m_energySystem_buildingsize_combobox;

	wxButton* m_energySystem_showSelected_button;
	wxButton* m_energySystem_showAll_button;

	wxComboBox* m_energySystem_system_type_combobox;
	wxTextCtrl* m_energySystem_system_name_textctrl;
	wxTextCtrl* m_energySystem_system_g1_textctrl;
	wxTextCtrl* m_energySystem_system_g2_textctrl;
	wxTextCtrl* m_energySystem_system_g3_textctrl;
	wxTextCtrl* m_energySystem_system_storage_textctrl;
	wxTextCtrl* m_energySystem_system_distribution_textctrl;
	wxTextCtrl* m_energySystem_system_aux_textctrl;		

	wxButton* m_energySystem_system_g1_button;
	wxButton* m_energySystem_system_g2_button;
	wxButton* m_energySystem_system_g3_button;
	wxButton* m_energySystem_system_storage_button;
	wxButton* m_energySystem_system_distribution_button;
	wxButton* m_energySystem_system_aux_button;

	wxDataViewListCtrl* m_energySystem_component_table;
	wxDataViewListCtrl* m_energySystem_system_table;

	wxTextCtrl* m_calculation_search_textctrl;
	wxTextCtrl* m_calculation_code_textctrl;
	wxTextCtrl* m_calculation_roof_uv_textctrl;
	wxTextCtrl* m_calculation_wall_uv_textctrl;
	wxTextCtrl* m_calculation_floor_uv_textctrl;
	wxTextCtrl* m_calculation_window_uv_textctrl;
	wxTextCtrl* m_calculation_door_uv_textctrl;
	wxButton* m_calculation_copyUv_button;

	wxDataViewListCtrl* m_calculation_set_table;

	std::set<std::string> m_countries_code;
	std::unordered_map<std::string, std::set<std::string>> m_regions_code;
};

