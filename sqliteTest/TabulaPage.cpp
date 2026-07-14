#include "wxprec.hpp"
#include <iostream>
#include <numeric>
#include <stdlib.h>
#include "sqlite3.h"

#include "utils.hpp"
#include "sqlTest_id.hpp"
#include "dataSources.hpp"
#include "TabulaPage.hpp"
#include "DetailDialog.hpp"
#include "country.hpp"

TabulaPage::TabulaPage(wxWindow* parent, sqlite3* db)
	: m_db(db)
{
	generate_countries_and_regions();
	getTypologyBuildingTypeNum();

	Create(parent, ID_TABULA_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

	m_notebook = new wxAuiNotebook(this, ID_TABULA_NOTEBOOK, wxDefaultPosition, wxDefaultSize, 0);

	// Typology Page

	m_typology_page = new wxPanel(m_notebook, ID_TABULA_TYPOLOGY_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxBoxSizer* typologyTopSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* typologySearchSizer = new wxBoxSizer(wxVERTICAL);
	typologySearchSizer->SetMinSize(260, 750);

	wxFlexGridSizer* searchGrid = new wxFlexGridSizer(0, 2, 0, 0);
	searchGrid->SetFlexibleDirection(wxBOTH);
	searchGrid->AddGrowableCol(0);
	searchGrid->AddGrowableCol(1);
	searchGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto search_source_text = new wxStaticText(m_typology_page, wxID_ANY, "Data source");
	searchGrid->Add(search_source_text, 0, wxALL | wxEXPAND, 5);

	auto search_source_text_content = new wxStaticText(m_typology_page, wxID_ANY, "TABULA (2017-06-01)");
	searchGrid->Add(search_source_text_content, 0, wxALL | wxEXPAND, 5);

	auto search_buildingUse_text = new wxStaticText(m_typology_page, wxID_ANY, "Building Use");
	searchGrid->Add(search_buildingUse_text, 0, wxALL | wxEXPAND, 5);

	auto search_buildingUse_content = new wxStaticText(m_typology_page, wxID_ANY, "Residential building");
	searchGrid->Add(search_buildingUse_content, 0, wxALL | wxEXPAND, 5);

	auto search_country_text = new wxStaticText(m_typology_page, wxID_ANY, "Country");
	searchGrid->Add(search_country_text, 0, wxALL | wxEXPAND, 5);

	m_typology_country_combobox = new wxComboBox(m_typology_page, ID_TABULA_TYPOLOGY_COUNTRY_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getCountryStrings(), 0);
	searchGrid->Add(m_typology_country_combobox, 0, wxALL | wxEXPAND, 5);

	auto search_year_text = new wxStaticText(m_typology_page, wxID_ANY, "Construction year");
	searchGrid->Add(search_year_text, 0, wxALL | wxEXPAND, 5);

	m_typology_year_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_YEAR_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_year_textctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	searchGrid->Add(m_typology_year_textctrl, 0, wxALL | wxEXPAND, 5);

	auto search_buildingSize_text = new wxStaticText(m_typology_page, wxID_ANY, "Building size class");
	searchGrid->Add(search_buildingSize_text, 0, wxALL | wxEXPAND, 5);

	m_typology_buildingSizeClass_combobox = new wxComboBox(m_typology_page, ID_TABULA_TYPOLOGY_BUILDINGSIZE_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getBuildingSizeStrings(), 0);
	searchGrid->Add(m_typology_buildingSizeClass_combobox, 0, wxALL | wxEXPAND, 5);

	auto search_buildingType_text = new wxStaticText(m_typology_page, wxID_ANY, "Building type");
	searchGrid->Add(search_buildingType_text, 0, wxALL | wxEXPAND, 5);

	m_typology_buildingType_combobox = new wxComboBox(m_typology_page, ID_TABULA_TYPOLOGY_BUILDINGTYPE_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getBuildingTypeStrings(), 0);
	searchGrid->Add(m_typology_buildingType_combobox, 0, wxALL | wxEXPAND, 5);

	auto search_building_text = new wxStaticText(m_typology_page, wxID_ANY, "Building");
	searchGrid->Add(search_building_text, 0, wxALL | wxEXPAND, 5);

	m_typology_building_combobox = new wxComboBox(m_typology_page, ID_TABULA_TYPOLOGY_BUILDING_COMBOBOX, "", wxDefaultPosition, wxDefaultSize);
	searchGrid->Add(m_typology_building_combobox, 0, wxALL | wxEXPAND, 5);

	auto search_elementtype_text = new wxStaticText(m_typology_page, wxID_ANY, "Element type");
	searchGrid->Add(search_elementtype_text, 0, wxALL | wxEXPAND, 5);

	m_typology_elementType_combobox = new wxComboBox(m_typology_page, ID_TABULA_TYPOLOGY_ELEMENT_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getElementTypeStrings(), 0);
	searchGrid->Add(m_typology_elementType_combobox, 0, wxALL | wxEXPAND, 5);

	auto searchResetButton = new wxButton(m_typology_page, ID_TABULA_TYPOLOGY_RESET_BUTTON, wxT("Reset"));
	searchGrid->Add(searchResetButton, 0, wxALL | wxEXPAND, 5);

	auto sizerForEnergySystem = new wxBoxSizer(wxHORIZONTAL);

	m_typology_searchHeat_button = new wxButton(m_typology_page, ID_TABULA_TYPOLOGY_SEARCH_HEAT_BUTTON, wxT("Heating system"));
	sizerForEnergySystem->Add(m_typology_searchHeat_button, 0, wxALL | wxEXPAND, 5);
	
	m_typology_searchWaterHeat_button = new wxButton(m_typology_page, ID_TABULA_TYPOLOGY_SEARCH_WATER_HEAT_BUTTON, wxT("Water heating system"));
	sizerForEnergySystem->Add(m_typology_searchWaterHeat_button, 0, wxALL | wxEXPAND, 5);

	searchGrid->Add(sizerForEnergySystem, 0, wxALL | wxEXPAND, 5);

	typologySearchSizer->Add(searchGrid, 0, wxALL | wxEXPAND, 5);

	m_typology_search_results_text = new wxStaticText(m_typology_page, wxID_ANY, "");
	m_typology_search_results_text->SetLabel(std::to_string(m_typology_building_type_num) + " entries of building type in total.");
	typologySearchSizer->Add(m_typology_search_results_text, 0, wxALL | wxEXPAND, 5);

	auto horizontalSplitLine1 = new wxStaticLine(m_typology_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	typologySearchSizer->Add(horizontalSplitLine1, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* typologyAverageSizer = new wxBoxSizer(wxVERTICAL);

	m_typology_average_buildingCode_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_MEAN_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_average_buildingCode_textctrl->Enable(false);
	typologyAverageSizer->Add(m_typology_average_buildingCode_textctrl, 0, wxALL | wxEXPAND, 5);

	wxFlexGridSizer* meanGrid = new wxFlexGridSizer(0, 2, 0, 0);

	meanGrid->SetFlexibleDirection(wxBOTH);
	meanGrid->AddGrowableCol(0);
	meanGrid->AddGrowableCol(1);
	meanGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto uv_roof_text = new wxStaticText(m_typology_page, wxID_ANY, "Roof u value");
	meanGrid->Add(uv_roof_text, 0, wxALL | wxEXPAND, 5);

	m_typology_average_roof_uv_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_MEAN_ROOF, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_average_roof_uv_textctrl->Enable(false);
	meanGrid->Add(m_typology_average_roof_uv_textctrl, 0, wxALL | wxEXPAND, 5);

	auto uv_wall_text = new wxStaticText(m_typology_page, wxID_ANY, "Wall u value");
	meanGrid->Add(uv_wall_text, 0, wxALL | wxEXPAND, 5);

	m_typology_average_wall_uv_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_MEAN_WALL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_average_wall_uv_textctrl->Enable(false);
	meanGrid->Add(m_typology_average_wall_uv_textctrl, 0, wxALL | wxEXPAND, 5);

	auto uv_floor_text = new wxStaticText(m_typology_page, wxID_ANY, "Floor u value");
	meanGrid->Add(uv_floor_text, 0, wxALL | wxEXPAND, 5);

	m_typology_average_floor_uv_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_MEAN_FLOOR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_average_floor_uv_textctrl->Enable(false);
	meanGrid->Add(m_typology_average_floor_uv_textctrl, 0, wxALL | wxEXPAND, 5);

	auto uv_door_text = new wxStaticText(m_typology_page, wxID_ANY, "Door u value");
	meanGrid->Add(uv_door_text, 0, wxALL | wxEXPAND, 5);

	m_typology_average_door_uv_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_MEAN_DOOR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_average_door_uv_textctrl->Enable(false);
	meanGrid->Add(m_typology_average_door_uv_textctrl, 0, wxALL | wxEXPAND, 5);

	auto uv_window_text = new wxStaticText(m_typology_page, wxID_ANY, "Window u value");
	meanGrid->Add(uv_window_text, 0, wxALL | wxEXPAND, 5);

	m_typology_average_window_uv_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_MEAN_WINDOW, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_average_window_uv_textctrl->Enable(false);
	meanGrid->Add(m_typology_average_window_uv_textctrl, 0, wxALL | wxEXPAND, 5);

	auto gv_window_text = new wxStaticText(m_typology_page, wxID_ANY, "Window g value");
	meanGrid->Add(gv_window_text, 0, wxALL | wxEXPAND, 5);

	m_typology_average_window_gv_textctrl = new wxTextCtrl(m_typology_page, ID_TABULA_TYPOLOGY_MEAN_WINDOW_G, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_typology_average_window_gv_textctrl->Enable(false);
	meanGrid->Add(m_typology_average_window_gv_textctrl, 0, wxALL | wxEXPAND, 5);

	m_typology_average_copyUV_button = new wxButton(m_typology_page, ID_TABULA_TYPOLOGY_COPY_UV_BUTTON, "Copy all U-values");
	meanGrid->Add(m_typology_average_copyUV_button, 0, wxALL | wxEXPAND, 5);

	typologyAverageSizer->Add(meanGrid, 0, wxALL | wxEXPAND, 5);

	typologySearchSizer->Add(typologyAverageSizer, 0, wxEXPAND | wxALL, 5);

	typologyTopSizer->Add(typologySearchSizer, 0, wxEXPAND | wxALL, 5);

	auto verticalSplitLine1 = new wxStaticLine(m_typology_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	typologyTopSizer->Add(verticalSplitLine1, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* typologyTableSizer = new wxBoxSizer(wxVERTICAL);

	m_typology_buildingType_table = new wxDataViewListCtrl(m_typology_page, ID_TABULA_TYPOLOGY_BUILDING_TYPE_TABLE);
	m_typology_buildingType_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	typologyTableSizer->Add(m_typology_buildingType_table, 1, wxALL | wxEXPAND, 5);

	m_typology_building_table = new wxDataViewListCtrl(m_typology_page, ID_TABULA_TYPOLOGY_BUILDING_TABLE);
	m_typology_building_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	typologyTableSizer->Add(m_typology_building_table, 1, wxALL | wxEXPAND, 5);

	m_typology_construction_table = new wxDataViewListCtrl(m_typology_page, ID_TABULA_TYPOLOGY_ELEMENT_TABLE);
	m_typology_construction_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	typologyTableSizer->Add(m_typology_construction_table, 1, wxALL | wxEXPAND, 5);

	typologyTopSizer->Add(typologyTableSizer, 1, wxEXPAND | wxALL, 5);

	m_typology_page->SetSizer(typologyTopSizer);
	m_typology_page->Layout();
	typologyTopSizer->Fit(m_typology_page);

	m_notebook->AddPage(m_typology_page, wxT("Typology"), false, wxNullBitmap);

	// energy system page

	m_energySystem_page = new wxPanel(m_notebook, ID_TABULA_ENERGYSYSTEM_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxBoxSizer* eSystemTopSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* eSystemSearchSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* eComponentGrid = new wxFlexGridSizer(0, 2, 0, 0);
	eComponentGrid->SetFlexibleDirection(wxBOTH);
	eComponentGrid->AddGrowableCol(0);
	eComponentGrid->AddGrowableCol(1);
	eComponentGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto es_source_text = new wxStaticText(m_energySystem_page, wxID_ANY, "Data source");
	eComponentGrid->Add(es_source_text, 0, wxALL | wxEXPAND, 5);

	auto es_source_text_content = new wxStaticText(m_energySystem_page, wxID_ANY, "TABULA (2017-06-01)");
	eComponentGrid->Add(es_source_text_content, 0, wxALL | wxEXPAND, 5);	

	auto es_country_text = new wxStaticText(m_energySystem_page, wxID_ANY, "Country");
	eComponentGrid->Add(es_country_text, 0, wxALL | wxEXPAND, 5);

	m_energySystem_country_combobox = new wxComboBox(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_COUNTRY_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getCountryStrings(), 0);
	eComponentGrid->Add(m_energySystem_country_combobox, 0, wxALL | wxEXPAND, 5);

	auto es_year_text = new wxStaticText(m_energySystem_page, wxID_ANY, "Construction year");
	eComponentGrid->Add(es_year_text, 0, wxALL | wxEXPAND, 5);

	m_energySystem_year_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_YEAR_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	m_energySystem_year_textctrl->SetValidator(wxTextValidator(wxFILTER_NUMERIC));
	eComponentGrid->Add(m_energySystem_year_textctrl, 0, wxALL | wxEXPAND, 5);

	auto es_buildingSizeClass_text = new wxStaticText(m_energySystem_page, wxID_ANY, "Building size class");
	eComponentGrid->Add(es_buildingSizeClass_text, 0, wxALL | wxEXPAND, 5);

	m_energySystem_buildingsize_combobox = new wxComboBox(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_BUILDINGSIZE_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getBuildingSizeStrings(), 0);
	eComponentGrid->Add(m_energySystem_buildingsize_combobox, 0, wxALL | wxEXPAND, 5);

	auto es_tables_text = new wxStaticText(m_energySystem_page, wxID_ANY, "Components");
	eComponentGrid->Add(es_tables_text, 0, wxALL | wxEXPAND, 5);

	m_energySystem_component_combobox = new wxComboBox(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_COMPONENT_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getEnergySystemTableStrings(), 0);
	m_energySystem_component_combobox->SetStringSelection("Heating Generator");
	eComponentGrid->Add(m_energySystem_component_combobox, 0, wxALL | wxEXPAND, 5);	

	m_energySystem_showSelected_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SHOW_FILTER_BUTTON, wxT("Filter energy system"));
	eComponentGrid->Add(m_energySystem_showSelected_button, 0, wxALL | wxEXPAND, 5);

	m_energySystem_showAll_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SHOW_ALL_BUTTON, wxT("Show all systems"));
	eComponentGrid->Add(m_energySystem_showAll_button, 0, wxALL | wxEXPAND, 5);

	eSystemSearchSizer->Add(eComponentGrid, 1, wxEXPAND | wxALL, 5);

	auto horizontalSplitLine2 = new wxStaticLine(m_energySystem_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	eSystemSearchSizer->Add(horizontalSplitLine2, 0, wxEXPAND | wxALL, 5);

	wxFlexGridSizer* eSystemGrid = new wxFlexGridSizer(0, 2, 0, 0);
	eSystemGrid->SetFlexibleDirection(wxBOTH);
	eSystemGrid->AddGrowableCol(0);
	eSystemGrid->AddGrowableCol(1);
	eSystemGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto heating_system_text = new wxStaticText(m_energySystem_page, wxID_ANY, "Heating system type");
	eSystemGrid->Add(heating_system_text, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_type_combobox = new wxComboBox(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_TYPE_COMBOBOX, "", wxDefaultPosition, wxDefaultSize, getEnergySystemsStrings(), 0);
	m_energySystem_system_type_combobox->SetStringSelection("Heating system");
	eSystemGrid->Add(m_energySystem_system_type_combobox, 0, wxALL | wxEXPAND, 5);

	auto es_name_text = new wxStaticText(m_energySystem_page, wxID_ANY, "System code");
	eSystemGrid->Add(es_name_text, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_name_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_NAME_TEXTCTRL);
	eSystemGrid->Add(m_energySystem_system_name_textctrl, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_g1_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G1_BUTTON, wxT("Generator 1"));
	eSystemGrid->Add(m_energySystem_system_g1_button, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_g1_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G1_TEXTCTRL);
	m_energySystem_system_g1_textctrl->Disable();
	eSystemGrid->Add(m_energySystem_system_g1_textctrl, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_g2_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G2_BUTTON, wxT("Generator 2"));
	eSystemGrid->Add(m_energySystem_system_g2_button, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_g2_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G2_TEXTCTRL);
	m_energySystem_system_g2_textctrl->Disable();
	eSystemGrid->Add(m_energySystem_system_g2_textctrl, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_g3_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G3_BUTTON, wxT("Generator 3"));
	eSystemGrid->Add(m_energySystem_system_g3_button, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_g3_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G3_TEXTCTRL);
	m_energySystem_system_g3_textctrl->Disable();
	eSystemGrid->Add(m_energySystem_system_g3_textctrl, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_storage_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_STORAGE_BUTTON, wxT("Storage"));
	eSystemGrid->Add(m_energySystem_system_storage_button, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_storage_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_STORAGE_TEXTCTRL);
	m_energySystem_system_storage_textctrl->Disable();
	eSystemGrid->Add(m_energySystem_system_storage_textctrl, 0, wxALL | wxEXPAND, 5);

	auto m_energySystem_system_distribution_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_DISTRIBUTION_BUTTON, wxT("Distribution"));
	eSystemGrid->Add(m_energySystem_system_distribution_button, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_distribution_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_DISTRIBUTION_TEXTCTRL);
	m_energySystem_system_distribution_textctrl->Disable();
	eSystemGrid->Add(m_energySystem_system_distribution_textctrl, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_aux_button = new wxButton(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_AUX_BUTTON, wxT("Auxiliary"));
	eSystemGrid->Add(m_energySystem_system_aux_button, 0, wxALL | wxEXPAND, 5);

	m_energySystem_system_aux_textctrl = new wxTextCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_AUX_TEXTCTRL);
	m_energySystem_system_aux_textctrl->Disable();
	eSystemGrid->Add(m_energySystem_system_aux_textctrl, 0, wxALL | wxEXPAND, 5);

	eSystemSearchSizer->Add(eSystemGrid, 1, wxEXPAND | wxALL, 5);

	eSystemTopSizer->Add(eSystemSearchSizer, 0, wxEXPAND | wxALL, 5);

	auto verticalSplitLine2 = new wxStaticLine(m_energySystem_page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
	eSystemTopSizer->Add(verticalSplitLine2, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* eSystemTableSizer = new wxBoxSizer(wxVERTICAL);

	m_energySystem_component_table = new wxDataViewListCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_COMPONENT_TABLE);
	m_energySystem_component_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	eSystemTableSizer->Add(m_energySystem_component_table, 1, wxALL | wxEXPAND, 5);

	m_energySystem_system_table = new wxDataViewListCtrl(m_energySystem_page, ID_TABULA_ENERGY_SYSTEM_SYSTEM_TABLE);
	m_energySystem_system_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	eSystemTableSizer->Add(m_energySystem_system_table, 1, wxALL | wxEXPAND, 5);

	eSystemTopSizer->Add(eSystemTableSizer, 1, wxALL | wxEXPAND, 5);

	m_energySystem_page->SetSizer(eSystemTopSizer);
	m_energySystem_page->Layout();
	eSystemTopSizer->Fit(m_energySystem_page);

	m_notebook->AddPage(m_energySystem_page, wxT("Energy system"), false, wxNullBitmap);

	// Calculation building set page

	m_calculation_page = new wxPanel(m_notebook, ID_TABULA_CALCULATION_PAGE, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxBoxSizer* calculationTopSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* calculationSearchSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* calculationFlexGrid = new wxFlexGridSizer(0, 2, 0, 0);
	calculationFlexGrid->SetFlexibleDirection(wxBOTH);
	calculationFlexGrid->AddGrowableCol(0);
	calculationFlexGrid->AddGrowableCol(1);
	calculationFlexGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	auto cal_source_text = new wxStaticText(m_calculation_page, wxID_ANY, "Data source");
	calculationFlexGrid->Add(cal_source_text, 0, wxALL | wxEXPAND, 5);

	auto cal_source_text_content = new wxStaticText(m_calculation_page, wxID_ANY, "TABULA (2017-06-01)");
	calculationFlexGrid->Add(cal_source_text_content, 0, wxALL | wxEXPAND, 5);

	auto cal_search_text = new wxStaticText(m_calculation_page, wxID_ANY, "Search");
	calculationFlexGrid->Add(cal_search_text, 0, wxALL | wxEXPAND, 5);

	m_calculation_search_textctrl = new wxTextCtrl(m_calculation_page, ID_TABULA_CALCULATION_SEARCH_TEXTCTRL);
	calculationFlexGrid->Add(m_calculation_search_textctrl, 0, wxALL | wxEXPAND, 5);

	auto cal_code_text = new wxStaticText(m_calculation_page, wxID_ANY, "Building variant code");
	calculationFlexGrid->Add(cal_code_text, 0, wxALL | wxEXPAND, 5);

	m_calculation_code_textctrl = new wxTextCtrl(m_calculation_page, ID_TABULA_CALCULATION_CODE_TEXTCTRL);
	m_calculation_code_textctrl->Disable();
	calculationFlexGrid->Add(m_calculation_code_textctrl, 0, wxALL | wxEXPAND, 5);

	auto cal_uv_roof_text = new wxStaticText(m_calculation_page, wxID_ANY, "Roof u value");
	calculationFlexGrid->Add(cal_uv_roof_text, 0, wxALL | wxEXPAND, 5);

	m_calculation_roof_uv_textctrl = new wxTextCtrl(m_calculation_page, ID_TABULA_CALCULATION_ROOF_UV_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	calculationFlexGrid->Add(m_calculation_roof_uv_textctrl, 0, wxALL | wxEXPAND, 5);
	m_calculation_roof_uv_textctrl->Enable(false);

	auto cal_uv_wall_text = new wxStaticText(m_calculation_page, wxID_ANY, "Wall u value");
	calculationFlexGrid->Add(cal_uv_wall_text, 0, wxALL | wxEXPAND, 5);

	m_calculation_wall_uv_textctrl = new wxTextCtrl(m_calculation_page, ID_TABULA_CALCULATION_WALL_UV_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	calculationFlexGrid->Add(m_calculation_wall_uv_textctrl, 0, wxALL | wxEXPAND, 5);
	m_calculation_wall_uv_textctrl->Enable(false);

	auto cal_uv_floor_text = new wxStaticText(m_calculation_page, wxID_ANY, "Floor u value");
	calculationFlexGrid->Add(cal_uv_floor_text, 0, wxALL | wxEXPAND, 5);

	m_calculation_floor_uv_textctrl = new wxTextCtrl(m_calculation_page, ID_TABULA_CALCULATION_FLOOR_UV_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	calculationFlexGrid->Add(m_calculation_floor_uv_textctrl, 0, wxALL | wxEXPAND, 5);
	m_calculation_floor_uv_textctrl->Enable(false);

	auto cal_uv_window_text = new wxStaticText(m_calculation_page, wxID_ANY, "Window u value");
	calculationFlexGrid->Add(cal_uv_window_text, 0, wxALL | wxEXPAND, 5);

	m_calculation_window_uv_textctrl = new wxTextCtrl(m_calculation_page, ID_TABULA_CALCULATION_WINDOW_UV_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	calculationFlexGrid->Add(m_calculation_window_uv_textctrl, 0, wxALL | wxEXPAND, 5);
	m_calculation_window_uv_textctrl->Enable(false);

	auto cal_uv_door_text = new wxStaticText(m_calculation_page, wxID_ANY, "Door u value");
	calculationFlexGrid->Add(cal_uv_door_text, 0, wxALL | wxEXPAND, 5);

	m_calculation_door_uv_textctrl = new wxTextCtrl(m_calculation_page, ID_TABULA_CALCULATION_DOOR_UV_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	calculationFlexGrid->Add(m_calculation_door_uv_textctrl, 0, wxALL | wxEXPAND, 5);
	m_calculation_door_uv_textctrl->Enable(false);

	m_calculation_copyUv_button = new wxButton(m_calculation_page, ID_TABULA_CALCULATION_COPY_UV_BUTTON, "Copy all U-values");
	calculationFlexGrid->Add(m_calculation_copyUv_button, 0, wxALL | wxEXPAND, 5);

	calculationSearchSizer->Add(calculationFlexGrid, 1, wxALL | wxEXPAND, 5);

	calculationTopSizer->Add(calculationSearchSizer, 1, wxEXPAND | wxALL, 5);

	wxBoxSizer* calculationTableSizer = new wxBoxSizer(wxVERTICAL);

	m_calculation_set_table = new wxDataViewListCtrl(m_calculation_page, ID_TABULA_CALCULATION_SET_TABLE);
	m_calculation_set_table->SetWindowStyle(wxDV_ROW_LINES | wxDV_HORIZ_RULES | wxDV_VERT_RULES);

	calculationTableSizer->Add(m_calculation_set_table, 1, wxALL | wxEXPAND, 5);

	calculationTopSizer->Add(calculationTableSizer, 3, wxEXPAND | wxALL, 5);

	m_calculation_page->SetSizer(calculationTopSizer);
	m_calculation_page->Layout();
	calculationTopSizer->Fit(m_calculation_page);

	m_notebook->AddPage(m_calculation_page, wxT("Calculation set"), false, wxNullBitmap);

	topSizer->Add(m_notebook, 1, wxALL | wxEXPAND, 5);

	showDB_buildingType();
	updateDB_buildingType();

	showDB_building();
	updateDB_building();

	showDB_construction();
	updateDB_construction();

	m_typology_buildingType_combobox->Set(getBuildingTypeStrings());
	m_typology_building_combobox->Set(getBuildingStrings());

	showDB_heating_generator();
	updateDB_heating_generator();

	showDB_heating_system();
	updateDB_heating_system();

	showDB_calculationSet();
	updateDB_calculationSet();

	BindEvent();

	this->SetSizer(topSizer);
	this->Layout();
	this->Centre(wxBOTH);
}

TabulaPage::~TabulaPage()
{
}

void TabulaPage::generate_countries_and_regions()
{
	std::string sentence = "SELECT * FROM " + TABULA_LOCATION;

	const char* sqlSentence = sentence.c_str();
	sqlite3_stmt* stmt = NULL;

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			auto country = transform(stmt, 2);

			m_countries_code.insert(country);

			if (m_regions_code.find(country) == m_regions_code.end())
			{
				m_regions_code.insert({ country, std::set<std::string>() });
				m_regions_code[country].insert(transform(stmt, 3));
			}
			else
			{
				m_regions_code[country].insert(transform(stmt, 3));
			}

		}
	}

	sqlite3_finalize(stmt);
}

wxArrayString TabulaPage::getCountryStrings()
{
	wxArrayString countries(1);

	countries.Add("");

	for (auto c : m_countries_code)
	{
		if (COUNTRIES_CODES.find(c) != COUNTRIES_CODES.end())
		{
			countries.Add(COUNTRIES_CODES.at(c) + "-" + c);
		}
	}

	return countries;
}

wxArrayString TabulaPage::getBuildingSizeStrings()
{
	wxArrayString types(1);

	types.Add("");
	types.Add("SFH-Single family house");
	types.Add("MFH-Multi family house");
	types.Add("TH-Terraced house");
	types.Add("AB-Apartment block");

	return types;
}

wxArrayString TabulaPage::getBuildingTypeStrings()
{
	wxArrayString types(1);

	types.Add("");

	if (m_typology_buildingType_table)
	{
		for (auto i = 0; i < m_typology_buildingType_table->GetItemCount(); ++i)
		{
			types.Add(m_typology_buildingType_table->GetTextValue(i, 1));
		}
	}
	return types;
}

wxArrayString TabulaPage::getElementTypeStrings()
{
	// todo : read from the DB?
	wxArrayString types(1);

	types.Add("");
	types.Add("Roof");
	types.Add("Ceiling");
	types.Add("Wall");
	types.Add("Floor");
	types.Add("Window");
	types.Add("Door");

	return types;
}

wxArrayString TabulaPage::getBuildingStrings()
{
	wxArrayString types(1);

	types.Add("");

	if (m_typology_buildingType_table)
	{
		for (auto i = 0; i < m_typology_building_table->GetItemCount(); ++i)
		{
			types.Add(m_typology_building_table->GetTextValue(i, 1));
		}
	}
	return types;
}

wxArrayString TabulaPage::getEnergySystemTableStrings()
{
	wxArrayString tables(1);

	tables.Add("Heating Generator");
	tables.Add("Heating Storage");
	tables.Add("Heating Distribution");
	tables.Add("Heating Auxiliary");
	tables.Add("Water Heating Generator");
	tables.Add("Water Heating Storage");
	tables.Add("Water Heating Distribution");
	tables.Add("Water Heating Auxiliary");

	return tables;
}

wxArrayString TabulaPage::getEnergySystemsStrings()
{
	wxArrayString types(1);

	types.Add("Heating system");
	types.Add("Water heating system");

	return types;
}

void TabulaPage::BindEvent()
{
	Bind(wxEVT_BUTTON, &TabulaPage::typology_Reset, this, ID_TABULA_TYPOLOGY_RESET_BUTTON);
	
	Bind(wxEVT_COMBOBOX, &TabulaPage::typology_selectCountry, this, ID_TABULA_TYPOLOGY_COUNTRY_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &TabulaPage::typology_selectBuildingSize, this, ID_TABULA_TYPOLOGY_BUILDINGSIZE_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &TabulaPage::typology_selectBuildingType, this, ID_TABULA_TYPOLOGY_BUILDINGTYPE_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &TabulaPage::typology_selectBuilding, this, ID_TABULA_TYPOLOGY_BUILDING_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &TabulaPage::typology_selectElementType, this, ID_TABULA_TYPOLOGY_ELEMENT_COMBOBOX);
	Bind(wxEVT_TEXT, &TabulaPage::typology_enterYear, this, ID_TABULA_TYPOLOGY_YEAR_TEXTCTRL);

	Bind(wxEVT_BUTTON, &TabulaPage::typology_searchHeatingSystem, this, ID_TABULA_TYPOLOGY_SEARCH_HEAT_BUTTON);
	Bind(wxEVT_BUTTON, &TabulaPage::typology_searchWaterHeatingSystem, this, ID_TABULA_TYPOLOGY_SEARCH_WATER_HEAT_BUTTON);

	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &TabulaPage::typology_doubleClick_buildingType, this, ID_TABULA_TYPOLOGY_BUILDING_TYPE_TABLE);
	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &TabulaPage::typology_doubleClick_buildingType, this, ID_TABULA_TYPOLOGY_BUILDING_TYPE_TABLE);

	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &TabulaPage::typology_doubleClick_building, this, ID_TABULA_TYPOLOGY_BUILDING_TABLE);
	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &TabulaPage::typology_doubleClick_building, this, ID_TABULA_TYPOLOGY_BUILDING_TABLE);

	Bind(wxEVT_BUTTON, &TabulaPage::typology_copyUV, this, ID_TABULA_TYPOLOGY_COPY_UV_BUTTON);

	Bind(wxEVT_COMBOBOX, &TabulaPage::energySystem_selectCountry, this, ID_TABULA_ENERGY_SYSTEM_COUNTRY_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &TabulaPage::energySystem_selectBuildingSize, this, ID_TABULA_ENERGY_SYSTEM_BUILDINGSIZE_COMBOBOX);
	Bind(wxEVT_TEXT, &TabulaPage::energySystem_enterYear, this, ID_TABULA_ENERGY_SYSTEM_YEAR_TEXTCTRL);

	Bind(wxEVT_COMBOBOX, &TabulaPage::energySystem_selectComponents, this, ID_TABULA_ENERGY_SYSTEM_COMPONENT_COMBOBOX);
	Bind(wxEVT_COMBOBOX, &TabulaPage::energySystem_selectSystem, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_TYPE_COMBOBOX);

	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_filterSystems, this, ID_TABULA_ENERGY_SYSTEM_SHOW_FILTER_BUTTON);
	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_showAllSystems, this, ID_TABULA_ENERGY_SYSTEM_SHOW_ALL_BUTTON);

	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &TabulaPage::energySystem_doubleClick_systems, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_TABLE);
	Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &TabulaPage::energySystem_filterSystems, this, ID_TABULA_ENERGY_SYSTEM_COMPONENT_TABLE);

	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_Generator1, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G1_BUTTON);
	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_Generator2, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G2_BUTTON);
	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_Generator3, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_G3_BUTTON);
	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_Storage, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_STORAGE_BUTTON);
	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_Distribution, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_DISTRIBUTION_BUTTON);
	Bind(wxEVT_BUTTON, &TabulaPage::energySystem_Auxiliary, this, ID_TABULA_ENERGY_SYSTEM_SYSTEM_AUX_BUTTON);

	Bind(wxEVT_TEXT, &TabulaPage::calculationBuildingSetSearch, this, ID_TABULA_CALCULATION_SEARCH_TEXTCTRL);
	Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &TabulaPage::selectCalculationSetBuilding, this, ID_TABULA_CALCULATION_SET_TABLE);
	Bind(wxEVT_BUTTON, &TabulaPage::onPressCopyCalculationUV, this, ID_TABULA_CALCULATION_COPY_UV_BUTTON);
}

void TabulaPage::getTypologyBuildingTypeNum()
{
	std::string sql = "SELECT COUNT(*) FROM " + TABULA_BUILDING_TYPE + ";";

	sqlite3_stmt* stmt = nullptr;
	int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
	if (rc != SQLITE_OK)
	{
		std::cerr << "sqlite3_prepare_v2 failed: " << sqlite3_errmsg(m_db) << std::endl;
		m_typology_building_type_num = 0;
	}

	int count = -1;

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW)
	{
		count = sqlite3_column_int(stmt, 0);
	}
	else
	{
		std::cerr << "sqlite3_step failed: " << sqlite3_errmsg(m_db) << std::endl;
	}

	sqlite3_finalize(stmt);
	m_typology_building_type_num = count;
}

void TabulaPage::typology_Reset(wxCommandEvent& evt)
{
	if (m_typology_country_combobox)
		m_typology_country_combobox->SetSelection(wxNOT_FOUND);

	if (m_typology_country_combobox)
		m_typology_country_combobox->SetSelection(wxNOT_FOUND);

	if (m_typology_buildingSizeClass_combobox)
		m_typology_buildingSizeClass_combobox->SetSelection(wxNOT_FOUND);

	if (m_typology_building_combobox)
		m_typology_building_combobox->SetSelection(wxNOT_FOUND);

	if (m_typology_elementType_combobox)
		m_typology_elementType_combobox->SetSelection(wxNOT_FOUND);

	if (m_typology_year_textctrl)
		m_typology_year_textctrl->SetValue('0');

	if (m_typology_average_buildingCode_textctrl)
		m_typology_average_buildingCode_textctrl->Clear();

	if (m_typology_average_roof_uv_textctrl)
		m_typology_average_roof_uv_textctrl->Clear();

	if (m_typology_average_floor_uv_textctrl)
		m_typology_average_floor_uv_textctrl->Clear();

	if (m_typology_average_wall_uv_textctrl)
		m_typology_average_wall_uv_textctrl->Clear();

	if (m_typology_average_window_uv_textctrl)
		m_typology_average_window_uv_textctrl->Clear();

	if (m_typology_average_door_uv_textctrl)
		m_typology_average_door_uv_textctrl->Clear();

	if (m_typology_average_window_gv_textctrl)
		m_typology_average_window_gv_textctrl->Clear();

	updateDB_buildingType();
	updateDB_building();
	updateDB_construction();

	if (m_energySystem_country_combobox)
		m_energySystem_country_combobox->SetSelection(wxNOT_FOUND);

	if (m_energySystem_year_textctrl)
		m_energySystem_year_textctrl->Clear();

	if (m_energySystem_buildingsize_combobox)
		m_energySystem_buildingsize_combobox->SetSelection(wxNOT_FOUND);

	updateDB_energySystemComponent();
	updateDB_energySystem();
}

void TabulaPage::showDB_buildingType()
{
	m_typology_buildingType_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("region", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("building_size_class_extension", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("average_reference_area", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("year_lass", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("year_class_extension", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("last_year extension", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("building_functionality", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_buildingType_table->AppendTextColumn("data_source", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_buildingType()
{
	m_typology_buildingType_table->DeleteAllItems();

	int num = 0;

	bool isCountryChosen(true), isYearGiven(true), isBuildingSizeChosen(true), isFunctionalityChosen(true);

	if (m_typology_country_combobox->GetSelection() == wxNOT_FOUND || m_typology_country_combobox->GetStringSelection() == "")
	{
		isCountryChosen = false;
	}

	if (m_typology_buildingSizeClass_combobox->GetSelection() == wxNOT_FOUND || m_typology_buildingSizeClass_combobox->GetStringSelection() == "")
	{
		isBuildingSizeChosen = false;
	}

	if (m_typology_year_textctrl->GetValue() == "")
	{
		isYearGiven = false;
	}

	std::string building_size = m_typology_buildingSizeClass_combobox->GetStringSelection().utf8_string();
	building_size = building_size.substr(0, building_size.find('-'));
	int year = isYearGiven ? std::stoi(m_typology_year_textctrl->GetValue().utf8_string()) : 0;

	std::string sentence = "SELECT * FROM " + TABULA_BUILDING_TYPE + " WHERE id NOTNULL";

	if (isCountryChosen)
	{
		std::string country = m_typology_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}
	if (isBuildingSizeChosen)
	{
		sentence += " AND (building_size_class IS " + quoteWord(building_size) + " OR building_size_class_ext IS " + quoteWord(building_size) + ")";
	}
	if (isYearGiven)
	{
		sentence += " AND first_year <= " + std::to_string(year) + " AND (CASE WHEN last_year_ext NOTNULL then last_year_ext >= " + std::to_string(year) + " ELSE last_year >= " + std::to_string(year) + " END)";
	}
	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			++num;
			wxVector<wxVariant> cells;

			for (int i = 0; i < 14; ++i)
			{
				if (i == 2)
					continue;
				cells.push_back(transform(stmt, i));
			}

			m_typology_buildingType_table->AppendItem(cells);
			m_typology_buildingType_table->TransferDataToWindow();
		}
	}
	sqlite3_finalize(stmt);

	if (num == m_typology_building_type_num)
	{
		m_typology_search_results_text->SetLabel(std::to_string(m_typology_building_type_num) + " entries of building type in total.");
	}
	else
	{
		m_typology_search_results_text->SetLabel("Find " + std::to_string(num) + " results from " + std::to_string(m_typology_building_type_num) + " building types.");
	}
}

void TabulaPage::showDB_building()
{
	m_typology_building_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("data_type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("reference_area", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_roof_1", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_roof_2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_wall_1", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_wall_2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_wall_3", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_floor_1", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_floor_2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_window_1", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_window_2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_building_table->AppendTextColumn("code_door_1", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);

}

void TabulaPage::updateDB_building()
{
	m_typology_building_table->DeleteAllItems();

	bool isBuildingTypeChosen(true); //isDatatypeChosen(true);

	if (m_typology_buildingType_combobox->GetSelection() == wxNOT_FOUND || m_typology_buildingType_combobox->GetStringSelection() == "")
	{
		return;
	}

	std::string building_type = m_typology_buildingType_combobox->GetStringSelection().utf8_string();
	
	std::string sentence = "SELECT * FROM " + TABULA_BUILDING + " WHERE id NOTNULL AND building_type IS " + quoteWord(building_type);

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 16; ++i)
			{
				if (i == 2)
				{
					continue;
				}

				cells.push_back(transform(stmt, i));
			}

			m_typology_building_table->AppendItem(cells);
			m_typology_building_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_construction()
{
	m_typology_construction_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("variant", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("element_type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("name", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("name_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("u_value", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("d_insulation", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_typology_construction_table->AppendTextColumn("g_value", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_construction()
{
	m_typology_construction_table->DeleteAllItems();

	bool isBuildingChosen(true), isElementChosen(true);

	if (m_typology_building_combobox->GetSelection() == wxNOT_FOUND || m_typology_building_combobox->GetStringSelection() == "")
	{
		return;
	}

	if (m_typology_elementType_combobox->GetSelection() == wxNOT_FOUND || m_typology_elementType_combobox->GetStringSelection() == "")
	{
		isElementChosen = false;
	}

	std::string building = m_typology_building_combobox->GetStringSelection().utf8_string();
	std::string element = m_typology_elementType_combobox->GetStringSelection().utf8_string();

	std::string sentence = "SELECT * FROM " + TABULA_CONSTRUCTION + " WHERE id NOTNULL";

	sentence += " AND (code in (SELECT code_roof_1 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_roof_2 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_wall_1 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_wall_2 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_wall_3 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_floor_1 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_floor_2 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_window_1 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_window_2 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + ")"\
		+ " OR code in (SELECT code_door_1 FROM " + TABULA_BUILDING + " WHERE code IS " + quoteWord(building) + "))";

	if (isElementChosen)
	{
		sentence += " AND element_type IS " + quoteWord(element);
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;

	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 14; ++i)
			{
				if (i == 2 || i == 9 || i == 10)
				{
					continue;
				}
				cells.push_back(transform(stmt, i));
			}

			m_typology_construction_table->AppendItem(cells);
			m_typology_construction_table->TransferDataToWindow();
		}
	}
	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_heating_generator()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("e_g_h_heat", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("e_g_h_electricity", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_heating_generator()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_HEATING_SYSTEM_GENERATOR + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 12; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_heating_storage()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_s_h", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_heating_storage()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_HEATING_SYSTEM_STORAGE + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 11; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_heating_distribution()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_d_h", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_heating_distribution()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_HEATING_SYSTEM_DISTRIBUTION + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 11; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_heating_aux()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_del_h_aux", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_heating_aux()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_HEATING_SYSTEM_AUXILIARY + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 11; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_water_heating_generator()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("e_g_w_heat", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("e_g_w_electricity", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_water_heating_generator()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_WATER_HEATING_SYSTEM_GENERATOR + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 12; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_water_heating_storage()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_s_w", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_s_w_h", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_water_heating_storage()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_WATER_HEATING_SYSTEM_STORAGE + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 12; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_water_heating_distribution()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_d_w", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_d_w_h", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_water_heating_distribution()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_WATER_HEATING_SYSTEM_DISTRIBUTION + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 12; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_water_heating_aux()
{
	m_energySystem_component_table->ClearColumns();

	m_energySystem_component_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("type", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("building_size_class", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("remark", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_component_table->AppendTextColumn("q_del_w_aux", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_water_heating_aux()
{
	m_energySystem_component_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_WATER_HEATING_SYSTEM_AUXILIARY + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (m_energySystem_year_textctrl->GetValue() != "")
	{
		auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
		sentence += " AND first_year <= " + year + " AND last_year >= " + year;
	}

	if (m_energySystem_buildingsize_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_buildingsize_combobox->GetStringSelection() != "")
	{
		auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
		bSize = bSize.substr(0, bSize.find('-'));
		if (bSize == "SFH" || bSize == "TH")
		{
			sentence += " AND (building_size_class IS 'SUH' OR building_size_class IS 'Gen')";
		}
		if (bSize == "MFH" || bSize == "AB")
		{
			sentence += " AND (building_size_class IS 'MUH' OR building_size_class IS 'Gen')";
		}
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 11; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_component_table->AppendItem(cells);
			m_energySystem_component_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_heating_system()
{
	m_energySystem_system_table->ClearColumns();

	m_energySystem_system_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("combination_e_carrier", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("combination_heat_generator", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_ec1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_ec2", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_g1", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_g2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_g3", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("fraction_g2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("fraction_g3", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_storage", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_distribution", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_aux", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_heating_system(const std::string& code)
{
	m_energySystem_system_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_HEATING_SYSTEM + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (code != "")
	{
		sentence += "AND (code_g1 IS " + quoteWord(code) \
			+ "or code_g2 IS " + quoteWord(code) \
			+ "or code_g3 IS " + quoteWord(code) \
			+ "or code_storage IS " + quoteWord(code) \
			+ "or code_distribution IS " + quoteWord(code) \
			+ "or code_aux IS " + quoteWord(code) + ")";
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 17; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_system_table->AppendItem(cells);
			m_energySystem_system_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::showDB_water_heating_system()
{
	m_energySystem_system_table->ClearColumns();

	m_energySystem_system_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("combination_e_carrier", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("combination_heat_generator", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_ec1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_ec2", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_ec3", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_g1", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_g2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_g3", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("fraction_g2", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("fraction_g3", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_storage", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_distribution", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_energySystem_system_table->AppendTextColumn("code_aux", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_water_heating_system(const std::string& code)
{
	m_energySystem_system_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_WATER_HEATING_SYSTEM + " WHERE id NOTNULL ";

	if (m_energySystem_country_combobox->GetSelection() != wxNOT_FOUND && m_energySystem_country_combobox->GetStringSelection() != "")
	{
		std::string country = m_energySystem_country_combobox->GetStringSelection().utf8_string().substr(0, 2);
		sentence += " AND country IS " + quoteWord(country);
	}

	if (code != "")
	{
		sentence += "AND (code_g1 IS " + quoteWord(code) \
			+ "or code_g2 IS " + quoteWord(code) \
			+ "or code_g3 IS " + quoteWord(code) \
			+ "or code_storage IS " + quoteWord(code) \
			+ "or code_distribution IS " + quoteWord(code) \
			+ "or code_aux IS " + quoteWord(code) + ")";
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 18; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_system_table->AppendItem(cells);
			m_energySystem_system_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::updateDB_energySystem()
{
	auto select = m_energySystem_system_type_combobox->GetStringSelection().utf8_string();

	if (select == "Heating system")
	{
		showDB_heating_system();
		updateDB_heating_system();
	}
	else if (select == "Water heating system")
	{
		showDB_water_heating_system();
		updateDB_water_heating_system();
	}
}

void TabulaPage::updateDB_energySystemComponent()
{
	auto select = m_energySystem_component_combobox->GetStringSelection().utf8_string();

	if (select == "Heating Generator")
	{
		showDB_heating_generator();
		updateDB_heating_generator();
	}
	else if (select == "Heating Storage")
	{
		showDB_heating_storage();
		updateDB_heating_storage();
	}
	else if (select == "Heating Distribution")
	{
		showDB_heating_distribution();
		updateDB_heating_distribution();
	}
	else if (select == "Heating Auxiliary")
	{
		showDB_heating_aux();
		updateDB_heating_aux();
	}
	else if (select == "Water Heating Generator")
	{
		showDB_water_heating_generator();
		updateDB_water_heating_generator();
	}
	else if (select == "Water Heating Storage")
	{
		showDB_water_heating_storage();
		updateDB_water_heating_storage();
	}
	else if (select == "Water Heating Distribution")
	{
		showDB_water_heating_distribution();
		updateDB_water_heating_distribution();
	}
	else if (select == "Water Heating Auxiliary")
	{
		showDB_water_heating_aux();
		updateDB_water_heating_aux();
	}
}

void TabulaPage::showDB_calculationSet()
{
	m_calculation_set_table->ClearColumns();

	m_calculation_set_table->AppendTextColumn("id", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("code", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("country", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("building", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("type_variant", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("description", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("description_national", wxDATAVIEW_CELL_INERT, TEXT_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("first_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("last_year", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("area", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_roof_1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_roof_2", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_wall_1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_wall_2", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_wall_3", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_floor_1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_floor_2", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_window_1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_window_1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
	m_calculation_set_table->AppendTextColumn("uv_door_1", wxDATAVIEW_CELL_INERT, VALUE_CELL_WIDTH, wxALIGN_LEFT, wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_RESIZABLE);
}

void TabulaPage::updateDB_calculationSet(const std::string& code)
{
	m_calculation_set_table->DeleteAllItems();

	std::string sentence = "SELECT * FROM " + TABULA_CALCULATION_BUILDING_SET + " WHERE id NOTNULL ";

	if (code != "")
	{
		sentence += "AND code LIKE " + searchText(code);
	}

	sentence += ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 34; ++i)
			{
				if (i < 7 || i == 11 || i == 12 || i == 13 || i > 23)
					cells.push_back(transform(stmt, i));
			}

			m_calculation_set_table->AppendItem(cells);
			m_calculation_set_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::typology_selectCountry(wxCommandEvent& evt)
{
	updateDB_buildingType();
	m_typology_buildingType_combobox->Set(getBuildingTypeStrings());
	m_typology_buildingType_combobox->SetStringSelection("");
	updateDB_building();
	m_typology_building_combobox->SetStringSelection("");
	updateDB_construction();

	auto country = m_typology_country_combobox->GetStringSelection().utf8_string();
	m_energySystem_country_combobox->SetStringSelection(country);

	updateDB_energySystemComponent();
	updateDB_energySystem();
}

void TabulaPage::typology_selectBuildingSize(wxCommandEvent& evt)
{
	updateDB_buildingType();
	m_typology_buildingType_combobox->Set(getBuildingTypeStrings());
	m_typology_buildingType_combobox->SetStringSelection("");
	updateDB_building();
	m_typology_building_combobox->SetStringSelection("");
	updateDB_construction();

	auto bSize = m_typology_buildingSizeClass_combobox->GetStringSelection().utf8_string();
	m_energySystem_buildingsize_combobox->SetStringSelection(bSize);

	updateDB_energySystemComponent();
	updateDB_energySystem();
}

void TabulaPage::typology_selectBuildingType(wxCommandEvent& evt)
{
	updateDB_building();
	m_typology_building_combobox->Set(getBuildingStrings());
	m_typology_building_combobox->SetStringSelection("");
	updateDB_construction();
}

void TabulaPage::typology_selectBuilding(wxCommandEvent& evt)
{
	updateDB_construction();
	auto str = m_typology_building_combobox->GetStringSelection();
	m_typology_average_buildingCode_textctrl->SetValue(str);
	calculateAverageOfBuilding();
}

void TabulaPage::typology_selectElementType(wxCommandEvent& evt)
{
	updateDB_construction();
}

void TabulaPage::typology_enterYear(wxCommandEvent& evt)
{
	updateDB_buildingType();
	m_typology_buildingType_combobox->Set(getBuildingTypeStrings());
	m_typology_buildingType_combobox->SetStringSelection("");
	updateDB_building();
	m_typology_building_combobox->SetStringSelection("");
	updateDB_construction();

	auto year = m_typology_year_textctrl->GetValue().utf8_string();
	m_energySystem_year_textctrl->SetValue(year);

	updateDB_energySystemComponent();
	updateDB_energySystem();
}

void TabulaPage::typology_doubleClick_buildingType(wxCommandEvent& evt)
{
	auto item = m_typology_buildingType_table->GetSelection();
	if (item != nullptr)
	{
		auto row = m_typology_buildingType_table->ItemToRow(item);
		auto str = m_typology_buildingType_table->GetTextValue(row, 1);
		m_typology_buildingType_combobox->SetStringSelection(str);
		updateDB_building();
		m_typology_building_combobox->Set(getBuildingStrings());
		m_typology_building_combobox->SetStringSelection("");
		updateDB_construction();
	}
}

void TabulaPage::typology_doubleClick_building(wxCommandEvent& evt)
{
	auto item = m_typology_building_table->GetSelection();
	if (item != nullptr)
	{
		auto row = m_typology_building_table->ItemToRow(item);
		auto str = m_typology_building_table->GetTextValue(row, 1);
		m_typology_building_combobox->SetStringSelection(str);
		updateDB_construction();
		m_typology_average_buildingCode_textctrl->SetValue(str);
		calculateAverageOfBuilding();
	}
}

void TabulaPage::typology_copyUV(wxCommandEvent& evt)
{
	auto code = m_typology_average_buildingCode_textctrl->GetValue().utf8_string();

	if (code != "")
	{
		auto roof_uv = m_typology_average_roof_uv_textctrl->GetValue().utf8_string();
		auto wall_uv = m_typology_average_wall_uv_textctrl->GetValue().utf8_string();
		auto floor_uv = m_typology_average_floor_uv_textctrl->GetValue().utf8_string();
		auto window_uv = m_typology_average_window_uv_textctrl->GetValue().utf8_string();
		auto door_uv = m_typology_average_door_uv_textctrl->GetValue().utf8_string();

		std::string content = "Building code: " + code + ";";

		if (roof_uv != "")
		{
			content += "roof u value: " + roof_uv + ";";
		}
		if (wall_uv != "")
		{
			content += "wall u value: " + wall_uv + ";";
		}
		if (floor_uv != "")
		{
			content += "floor u value: " + floor_uv + ";";
		}
		if (window_uv != "")
		{
			content += "window u value: " + window_uv + ";";
		}
		if (door_uv != "")
		{
			content += "door u value: " + door_uv + ";";
		}

		auto clipboard = wxClipboard::Get();
		clipboard->Open();

		if (clipboard->IsOpened())
		{
			clipboard->Clear();
			clipboard->SetData(new wxTextDataObject(wxString::FromUTF8(content.c_str())));
			clipboard->Flush();
			clipboard->Close();

			const wxString msg = "U_values of constructions have been copied to clipboard.";
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK);
		}
	}
}

void TabulaPage::typology_searchHeatingSystem(wxCommandEvent& evt)
{
	m_notebook->SetSelection(1);
	m_energySystem_system_type_combobox->SetStringSelection("Heating system");

	showDB_heating_system();
	searchHeatingSystem();
}

void TabulaPage::typology_searchWaterHeatingSystem(wxCommandEvent& evt)
{
	m_notebook->SetSelection(1);
	m_energySystem_system_type_combobox->SetStringSelection("Water heating system");

	showDB_water_heating_system();
	searchWaterHeatingSystem();
}

void TabulaPage::searchHeatingSystem()
{
	auto building_type_selection = m_typology_buildingType_table->GetSelection();

	if (building_type_selection == nullptr)
	{
		return;
	}

	auto row = m_typology_buildingType_table->ItemToRow(building_type_selection);

	auto region = m_typology_buildingType_table->GetTextValue(row, 2);

	auto country = region.substr(0, region.find('.')).utf8_string();

	auto size_class = m_typology_buildingType_table->GetTextValue(row, 3).utf8_string();

	std::string size_catalog;

	if (size_class == "SFH" || size_class == "TH")
		size_catalog = "SUH";
	else if (size_class == "MFH" || size_class == "AB")
		size_catalog = "MUH";

	std::string size_condition = "building_size_class IS " + quoteWord(size_catalog);

	auto first_year = m_typology_buildingType_table->GetTextValue(row, 8).utf8_string();

	auto last_year = m_typology_buildingType_table->GetTextValue(row, 9).utf8_string();

	std::string year_condition = "NOT ((first_year > " + last_year + ") OR (last_year < " + first_year + "))";

	std::string sentence = "SELECT * FROM system_heating_system WHERE country IS " + quoteWord(country);

	sentence += " AND ";

	sentence += "code_g1 IN (SELECT code FROM system_heat_generator WHERE " + size_condition + " AND " + year_condition + ") AND (";

	sentence += "(code_g2 is NULL or code_g2 IN (SELECT code FROM system_heat_generator WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_g3 is NULL or code_g3 IN (SELECT code FROM system_heat_generator WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_storage is NULL OR code_storage IN (SELECT code FROM system_heat_storage WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_distribution is NULL OR code_distribution IN (SELECT code FROM system_heat_distribution WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_aux is NULL OR code_aux IN (SELECT code FROM system_heat_auxiliary WHERE " + size_condition + " AND " + year_condition + ")))";

	sentence += ";";

	m_energySystem_system_table->DeleteAllItems();

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 17; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_system_table->AppendItem(cells);
			m_energySystem_system_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::searchWaterHeatingSystem()
{
	auto building_type_selection = m_typology_buildingType_table->GetSelection();

	if (building_type_selection == nullptr)
	{
		return;
	}

	auto row = m_typology_buildingType_table->ItemToRow(building_type_selection);

	auto region = m_typology_buildingType_table->GetTextValue(row, 2);

	auto country = region.substr(0, region.find('.')).utf8_string();

	auto size_class = m_typology_buildingType_table->GetTextValue(row, 3).utf8_string();

	std::string size_catalog;

	if (size_class == "SFH" || size_class == "TH")
		size_catalog = "SUH";
	else if (size_class == "MFH" || size_class == "AB")
		size_catalog = "MUH";

	std::string size_condition = "building_size_class IS " + quoteWord(size_catalog);

	auto first_year = m_typology_buildingType_table->GetTextValue(row, 8).utf8_string();

	auto last_year = m_typology_buildingType_table->GetTextValue(row, 9).utf8_string();

	std::string year_condition = "NOT ((first_year > " + last_year + ") OR (last_year < " + first_year + "))";

	std::string sentence = "SELECT * FROM system_water_heating_system WHERE country IS " + quoteWord(country);

	sentence += " AND ";

	sentence += "code_g1 IN (SELECT code FROM system_water_generator WHERE " + size_condition + " AND " + year_condition + ") AND ";

	sentence += "(code_g2 IS NULL OR code_g2 IN (SELECT code FROM system_water_generator WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_g3 IS NULL OR code_g3 IN (SELECT code FROM system_water_generator WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_storage is NULL OR code_storage IN (SELECT code FROM system_water_storage WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_distribution is NULL OR code_distribution IN (SELECT code FROM system_water_distribution WHERE " + size_condition + " AND " + year_condition + ")) AND ";
	sentence += "(code_aux is NULL OR code_aux IN (SELECT code FROM system_water_auxiliary WHERE " + size_condition + " AND " + year_condition + ")))";

	sentence += ";";

	m_energySystem_system_table->DeleteAllItems();

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			wxVector<wxVariant> cells;

			for (int i = 0; i < 17; ++i)
			{
				cells.push_back(transform(stmt, i));
			}

			m_energySystem_system_table->AppendItem(cells);
			m_energySystem_system_table->TransferDataToWindow();
		}
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::energySystem_selectComponents(wxCommandEvent& evt)
{
	updateDB_energySystemComponent();
	updateDB_energySystem();
}

void TabulaPage::energySystem_selectCountry(wxCommandEvent& evt)
{
	auto country = m_energySystem_country_combobox->GetStringSelection().utf8_string();
	m_typology_country_combobox->SetStringSelection(country);

	updateDB_energySystemComponent();
	updateDB_energySystem();

	updateDB_buildingType();
	m_typology_buildingType_combobox->Set(getBuildingTypeStrings());
	m_typology_buildingType_combobox->SetStringSelection("");
	updateDB_building();
	m_typology_building_combobox->SetStringSelection("");
	updateDB_construction();
}

void TabulaPage::energySystem_enterYear(wxCommandEvent& evt)
{
	updateDB_energySystemComponent();
	updateDB_energySystem();

	auto year = m_energySystem_year_textctrl->GetValue().utf8_string();
	m_typology_year_textctrl->SetValue(year);

	updateDB_buildingType();
	m_typology_buildingType_combobox->Set(getBuildingTypeStrings());
	m_typology_buildingType_combobox->SetStringSelection("");
	updateDB_building();
	m_typology_building_combobox->SetStringSelection("");
	updateDB_construction();
}

void TabulaPage::energySystem_selectBuildingSize(wxCommandEvent& evt)
{
	updateDB_energySystemComponent();
	updateDB_energySystem();

	auto bSize = m_energySystem_buildingsize_combobox->GetStringSelection().utf8_string();
	m_typology_buildingSizeClass_combobox->SetStringSelection(bSize);

	updateDB_buildingType();
	m_typology_buildingType_combobox->Set(getBuildingTypeStrings());
	m_typology_buildingType_combobox->SetStringSelection("");
	updateDB_building();
	m_typology_building_combobox->SetStringSelection("");
	updateDB_construction();
}

void TabulaPage::energySystem_filterSystems(wxCommandEvent& evt)
{
	auto tables = m_energySystem_component_combobox->GetStringSelection().utf8_string();
	auto selection = m_energySystem_component_table->GetSelection();

	if (selection != nullptr)
	{
		auto row = m_energySystem_component_table->ItemToRow(selection);
		auto str = m_energySystem_component_table->GetTextValue(row, 1).utf8_string();

		if (tables == "Heating Generator" || tables == "Heating Storage" || tables == "Heating Distribution" || tables == "Heating Auxiliary")
		{
			m_energySystem_system_type_combobox->SetStringSelection("Heating system");
			showDB_heating_system();
			updateDB_heating_system(str);
		}
		else
		{
			m_energySystem_system_type_combobox->SetStringSelection("Water heating system");
			showDB_water_heating_system();
			updateDB_water_heating_system(str);
		}
	}
}

void TabulaPage::energySystem_showAllSystems(wxCommandEvent& evt)
{
	if (m_energySystem_system_type_combobox->GetStringSelection().utf8_string() == "Heating system")
	{
		showDB_heating_system();
		updateDB_heating_system();
	}
	else if (m_energySystem_system_type_combobox->GetStringSelection().utf8_string() == "Water heating system")
	{
		showDB_water_heating_system();
		updateDB_water_heating_system();
	}
}

void TabulaPage::energySystem_selectSystem(wxCommandEvent& evt)
{
	updateDB_energySystem();
}

void TabulaPage::energySystem_doubleClick_systems(wxDataViewEvent& evt)
{
	auto selection = m_energySystem_system_table->GetSelection();
	auto h_system = m_energySystem_system_type_combobox->GetStringSelection().utf8_string();

	if (selection != nullptr)
	{
		if (h_system == "Heating system")
		{
			auto row = m_energySystem_system_table->ItemToRow(selection);
			auto code = m_energySystem_system_table->GetTextValue(row, 1);
			auto g1 = m_energySystem_system_table->GetTextValue(row, 9);
			auto g2 = m_energySystem_system_table->GetTextValue(row, 10);
			auto g3 = m_energySystem_system_table->GetTextValue(row, 11);
			auto storage = m_energySystem_system_table->GetTextValue(row, 14);
			auto distribution = m_energySystem_system_table->GetTextValue(row, 15);
			auto aux = m_energySystem_system_table->GetTextValue(row, 16);

			m_energySystem_system_name_textctrl->SetValue(code);
			m_energySystem_system_g1_textctrl->SetValue(g1);
			m_energySystem_system_g2_textctrl->SetValue(g2);
			m_energySystem_system_g3_textctrl->SetValue(g3);
			m_energySystem_system_storage_textctrl->SetValue(storage);
			m_energySystem_system_distribution_textctrl->SetValue(distribution);
			m_energySystem_system_aux_textctrl->SetValue(aux);
		}
		else if (h_system == "Water heating system")
		{
			auto row = m_energySystem_system_table->ItemToRow(selection);
			auto code = m_energySystem_system_table->GetTextValue(row, 1);
			auto g1 = m_energySystem_system_table->GetTextValue(row, 10);
			auto g2 = m_energySystem_system_table->GetTextValue(row, 11);
			auto g3 = m_energySystem_system_table->GetTextValue(row, 12);
			auto storage = m_energySystem_system_table->GetTextValue(row, 15);
			auto distribution = m_energySystem_system_table->GetTextValue(row, 16);
			auto aux = m_energySystem_system_table->GetTextValue(row, 17);

			m_energySystem_system_name_textctrl->SetValue(code);
			m_energySystem_system_g1_textctrl->SetValue(g1);
			m_energySystem_system_g2_textctrl->SetValue(g2);
			m_energySystem_system_g3_textctrl->SetValue(g3);
			m_energySystem_system_storage_textctrl->SetValue(storage);
			m_energySystem_system_distribution_textctrl->SetValue(distribution);
			m_energySystem_system_aux_textctrl->SetValue(aux);
		}
	}
}

void TabulaPage::calculationBuildingSetSearch(wxCommandEvent& evt)
{
	auto code = m_calculation_search_textctrl->GetValue().utf8_string();
	updateDB_calculationSet(code);
}

void TabulaPage::onPressCopyCalculationUV(wxCommandEvent& evt)
{
	copyCalculatedUVtoClipbrd();
}

void TabulaPage::selectCalculationSetBuilding(wxCommandEvent& evt)
{
	auto str = m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 1);

	auto item = m_calculation_set_table->GetSelection();

	calculateAverageUVofrefurbishedBuilding(item);

	m_calculation_code_textctrl->SetValue(str);
}

void TabulaPage::calculateAverageUVofrefurbishedBuilding(wxDataViewItem& item)
{
	std::vector<double> roofs, floors, walls, doors, windows, windows_g;
	double mean_roof(0.0), mean_floor(0.0), mean_wall(0.0), mean_door(0.0), mean_window(0.0);

	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 10).ToStdString() != "0.0")
	{
		roofs.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 10).ToStdString()));
	}
	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 11).ToStdString() != "0.0")
	{
		roofs.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 11).ToStdString()));
	}

	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 12).ToStdString() != "0.0")
	{
		walls.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 12).ToStdString()));
	}
	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 13).ToStdString() != "0.0")
	{
		walls.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 13).ToStdString()));
	}
	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 14).ToStdString() != "0.0")
	{
		walls.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 14).ToStdString()));
	}

	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 15).ToStdString() != "0.0")
	{
		floors.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 15).ToStdString()));
	}
	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 16).ToStdString() != "0.0")
	{
		floors.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 16).ToStdString()));
	}

	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 17).ToStdString() != "0.0")
	{
		windows.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 17).ToStdString()));
	}
	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 18).ToStdString() != "0.0")
	{
		windows.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 18).ToStdString()));
	}

	if (m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 19).ToStdString() != "0.0")
	{
		doors.push_back(std::stod(m_calculation_set_table->GetTextValue(m_calculation_set_table->GetSelectedRow(), 19).ToStdString()));
	}

	auto mean = [](auto vectors)
		{
			double sum = std::accumulate(vectors.begin(), vectors.end(), 0.00);
			return vectors.size() == 0 ? 0.0 : double(sum / vectors.size());
		};

	mean_roof = mean(roofs);
	mean_floor = mean(floors);
	mean_wall = mean(walls);
	mean_door = mean(doors);
	mean_window = mean(windows);

	m_calculation_roof_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_roof));

	m_calculation_floor_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_floor));

	m_calculation_wall_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_wall));

	m_calculation_door_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_door));

	m_calculation_window_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_window));
}

void TabulaPage::copyCalculatedUVtoClipbrd()
{
	auto code = m_calculation_code_textctrl->GetValue().utf8_string();

	if (code != "")
	{
		auto roof_uv = m_calculation_roof_uv_textctrl->GetValue().utf8_string();
		auto wall_uv = m_calculation_wall_uv_textctrl->GetValue().utf8_string();
		auto floor_uv = m_calculation_floor_uv_textctrl->GetValue().utf8_string();
		auto window_uv = m_calculation_window_uv_textctrl->GetValue().utf8_string();
		auto door_uv = m_calculation_door_uv_textctrl->GetValue().utf8_string();

		std::string content = "Building code: " + code + ";";

		if (roof_uv != "")
		{
			content += "roof u value: " + roof_uv + ";";
		}
		if (wall_uv != "")
		{
			content += "wall u value: " + wall_uv + ";";
		}
		if (floor_uv != "")
		{
			content += "floor u value: " + floor_uv + ";";
		}
		if (window_uv != "")
		{
			content += "window u value: " + window_uv + ";";
		}
		if (door_uv != "")
		{
			content += "door u value: " + door_uv + ";";
		}

		auto clipboard = wxClipboard::Get();
		clipboard->Open();

		if (clipboard->IsOpened())
		{
			clipboard->Clear();
			clipboard->SetData(new wxTextDataObject(wxString::FromUTF8(content.c_str())));
			clipboard->Flush();
			clipboard->Close();

			const wxString msg = "U_values of constructions have been copied to clipboard.";
			wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK);
		}
	}
}

void TabulaPage::calculateAverageOfBuilding()
{
	std::vector<double> ceilings, roofs, floors, walls, doors, windows, windows_g;
	double mean_ceiling, mean_roof, mean_floor, mean_wall, mean_door, mean_window, mean_window_g;

	for (int i = 0; i < m_typology_construction_table->GetItemCount(); ++i)
	{
		auto e_type = m_typology_construction_table->GetTextValue(i, 3);
		auto uv = std::stod(m_typology_construction_table->GetTextValue(i, 8).ToStdString());

		if (e_type == "Ceiling")
		{
			ceilings.push_back(uv);
		}
		else if (e_type == "Roof")
		{
			roofs.push_back(uv);
		}
		else if (e_type == "Floor")
		{
			floors.push_back(uv);
		}
		else if (e_type == "Wall")
		{
			walls.push_back(uv);
		}
		else if (e_type == "Door")
		{
			doors.push_back(uv);
		}
		else if (e_type == "Window")
		{
			windows.push_back(uv);
			windows_g.push_back(std::stod(m_typology_construction_table->GetTextValue(i, 10).ToStdWstring()));
		}
	}

	auto mean = [](auto vectors)
		{
			double sum = std::accumulate(vectors.begin(), vectors.end(), 0.00);
			return vectors.size() == 0 ? 0.0 : double(sum / vectors.size());
		};

	mean_ceiling = mean(ceilings);
	mean_roof = mean(roofs);
	mean_floor = mean(floors);
	mean_wall = mean(walls);
	mean_door = mean(doors);
	mean_window = mean(windows);
	mean_window_g = mean(windows_g);

	if (roofs.empty())
		m_typology_average_roof_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_ceiling));
	else
		m_typology_average_roof_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_roof));

	m_typology_average_floor_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_floor));

	m_typology_average_wall_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_wall));

	m_typology_average_door_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_door));

	m_typology_average_window_uv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_window));

	m_typology_average_window_gv_textctrl->SetValue(wxString::Format(wxT("%.2f"), mean_window_g));
}

void TabulaPage::energySystem_showComponentDetails(const std::string& type, const std::string& code)
{
	auto waterOrNot = m_energySystem_system_type_combobox->GetStringSelection().utf8_string() == "Water heating system" ? true : false;

	int numOfColumn = 0;
	std::string sentence = "SELECT * FROM ";

	if (waterOrNot)
	{
		sentence += TABULA_WATER_HEATING_SYSTEM;
	}
	else
	{
		sentence += TABULA_HEATING_SYSTEM;
	}

	if (type == "Generator")
	{
		sentence += "_generator ";
		numOfColumn = 12;
	}
	else if (type == "Storage")
	{
		sentence += "_storage ";
		numOfColumn = waterOrNot ? 12 : 11;
	}
	else if (type == "Distribution")
	{
		sentence += "_distribution ";
		numOfColumn = waterOrNot ? 12 : 11;
	}
	else if (type == "Auxiliary")
	{
		sentence += "_auxiliary ";
		numOfColumn = 11;
	}

	sentence += "WHERE id NOTNULL AND code IS " + quoteWord(code) + ";";

	sqlite3_stmt* stmt = NULL;
	const char* sqlSentence = sentence.c_str();

	std::vector<std::string> details;

	int result = sqlite3_prepare_v2(m_db, sqlSentence, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			for (int i = 1; i < numOfColumn; ++i)
			{
				details.push_back(transform(stmt, i));
			}
		}

		if (details.size() == 0)
		{
			return;
		}

		std::vector<std::pair<std::string, std::string>> infos;

		infos.push_back({ "Code:", details[0] });
		infos.push_back({ "Country:", details[1] });
		infos.push_back({ "Type:", details[2] });
		infos.push_back({ "Building size class:", details[3] });
		infos.push_back({ "Description:", details[4] });
		infos.push_back({ "Description national:", details[5] });
		infos.push_back({ "Remark:", details[6] });
		infos.push_back({ "First year:", details[7] });
		infos.push_back({ "Last year:", details[8] });

		if (type == "Generator")
		{
			if (waterOrNot)
			{
				infos.push_back({ "e_g_w_Heat:", details[9] });
				infos.push_back({ "e_g_w_Electricity", details[10] });
			}
			else
			{
				infos.push_back({ "e_g_h_Heat:", details[9] });
				infos.push_back({ "e_g_h_Electricity", details[10] });
			}
		}
		else if (type == "Storage")
		{
			if (waterOrNot)
			{
				infos.push_back({ "q_s_w:", details[9] });
				infos.push_back({ "q_s_w_h", details[10] });
			}
			else
			{
				infos.push_back({ "q_s_h:", details[9] });
			}
		}
		else if (type == "Distribution")
		{
			if (waterOrNot)
			{
				infos.push_back({ "q_d_w:", details[9] });
				infos.push_back({ "q_d_w_h", details[10] });
			}
			else
			{
				infos.push_back({ "q_d_h:", details[9] });
			}
		}
		else if (type == "Auxiliary")
		{
			if (waterOrNot)
			{
				infos.push_back({ "q_del_w_aux:", details[9] });
			}
			else
			{
				infos.push_back({ "q_del_h_aux:", details[9] });
			}
		}

		auto dialog = new DetailDialog(this, infos);
		dialog->ShowModal();
	}

	sqlite3_finalize(stmt);
}

void TabulaPage::energySystem_Generator1(wxCommandEvent& evt)
{
	energySystem_showComponentDetails("Generator", m_energySystem_system_g1_textctrl->GetValue().utf8_string());
}

void TabulaPage::energySystem_Generator2(wxCommandEvent& evt)
{
	energySystem_showComponentDetails("Generator", m_energySystem_system_g2_textctrl->GetValue().utf8_string());
}

void TabulaPage::energySystem_Generator3(wxCommandEvent& evt)
{
	energySystem_showComponentDetails("Generator", m_energySystem_system_g3_textctrl->GetValue().utf8_string());
}

void TabulaPage::energySystem_Storage(wxCommandEvent& evt)
{
	energySystem_showComponentDetails("Storage", m_energySystem_system_storage_textctrl->GetValue().utf8_string());
}

void TabulaPage::energySystem_Distribution(wxCommandEvent& evt)
{
	energySystem_showComponentDetails("Distribution", m_energySystem_system_distribution_textctrl->GetValue().utf8_string());
}

void TabulaPage::energySystem_Auxiliary(wxCommandEvent& evt)
{
	energySystem_showComponentDetails("Auxiliary", m_energySystem_system_aux_textctrl->GetValue().utf8_string());
}
