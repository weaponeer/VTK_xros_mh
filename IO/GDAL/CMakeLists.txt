vtk_module_find_package(PRIVATE_IF_SHARED
  PACKAGE GDAL)

set(classes
  vtkGDAL
  vtkGDALRasterReader
  vtkGDALVectorReader)

vtk_module_add_module(VTK::IOGDAL
  CLASSES ${classes})
vtk_module_link(VTK::IOGDAL
  PRIVATE
    GDAL::GDAL)
vtk_add_test_mangling(VTK::IOGDAL)
