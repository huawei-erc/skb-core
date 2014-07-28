
add_library(icparc_solver bitmap.c edge_finder.c eregex.c ic.c)

install (TARGETS icparc_solver DESTINATION bin)
install (FILES ast.h bitmap.h eplex_params.h DESTINATION include)

