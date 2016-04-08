#include <opm/parser/eclipse/Parser/Parser.hpp>
#include <opm/parser/eclipse/Parser/ParseContext.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>

#include <opm/core/grid.h>
#include <opm/core/grid/cornerpoint_grid.h>
#include <opm/core/grid/GridManager.hpp>
#include <opm/autodiff/GridHelpers.hpp>
#include <opm/simulators/thresholdPressures.hpp> // Note: the GridHelpers must be included before this (to make overloads available)


static DeckPtr createDeckSimConfig() {
    const std::string& inputStr = "RUNSPEC\n"
                                  "EQLOPTS\n"
                                  "THPRES /\n "
                                  "DIMENS\n"
                                  "10 3 4 /\n"
                                  "\n"
                                  "GRID\n"
                                  "REGIONS\n"
                                  "EQLNUM\n"
                                  "10*1 10*2 100*3 /\n "
                                  "\n"

                                  "SOLUTION\n"
                                  "THPRES\n"
                                  "1 2 12.0/\n"
                                  "1 3 5.0/\n"
                                  "2 3 7.0/\n"
                                  "/\n"
                                  "\n";


    ParserPtr parser(new Parser());
    return parser->parseString(inputStr, ParseContext()) ;
}

/*
  The main purpose of this is to get the code in
  thresholdPressures.hpp to compile; it currently has no users in the
  opm-core codebase.
*/

BOOST_AUTO_TEST_CASE(CreateSimulationConfig) {
    ParseContext parseContext;
    typedef UnstructuredGrid  Grid;
    DeckPtr deck = createDeckSimConfig();
    EclipseState state(deck, parseContext);
    EclipseGridConstPtr eclipseGrid = state.getEclipseGrid();
    std::vector<double> porv = eclipseState->getDoubleGridProperty("PORV")->getData();
    GridManager gridManager( eclipseState->getEclipseGrid(), porv );
    const Grid& grid = *(gridManager.c_grid());

    std::vector<double> threshold_pressures = thresholdPressures(parseContext, eclipseState, grid);
    BOOST_CHECK( threshold_pressures.size() > 0 );
}
