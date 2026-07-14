building_type
(
id,
location_id,
construction_year_id,
name,
description,
)

location
(
id,
country,
region
)

construction_year
(
id,
code,
since,
until
)

// construction_year in tabula.values.Tab.Building.Constr may differ from tabula.values.Tab.ConstrYearClass, in Tab.Building.Constr there is overlap between the time intervals, in Tab.ConstrYearClass the time invervals in different classes are distinct, which is chosen to be written in the databank

construction
(
id,
building_type_id,
variant,
elemnt_type, 				# wall, slab, roof, ...
name,
description,
glazing_ratio
)

construction_layer
(
id,
material_id,
construction_id,
name,
description,
thickness,
u_value,
g_value,
fraction
)

material
(
id,
name,
description,
conductivity,
density,
specificHeat
)
