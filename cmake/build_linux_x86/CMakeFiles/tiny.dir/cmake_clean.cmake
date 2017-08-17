file(REMOVE_RECURSE
  "output/libtiny.pdb"
  "output/libtiny.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/tiny.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
