file(REMOVE_RECURSE
  "output/libtiny.pdb"
  "output/libtiny.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tiny_static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
