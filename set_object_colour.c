#include  <mni.h>

int  main(
    int   argc,
    char  *argv[] )
{
    char                 *src_filename, *dest_filename, *colour_name;
    File_formats         format;
    Real                 line_thickness;
    int                  i;
    Colour               colour;
    int                  n_objects;
    object_struct        **objects;

    initialize_argument_processing( argc, argv );

    if( !get_string_argument( "", &src_filename ) ||
        !get_string_argument( "", &dest_filename ) ||
        !get_string_argument( "", &colour_name ) ||
        !lookup_colour( colour_name, &colour ) )
    {
        print( "Usage: %s  src  dest  colour\n", argv[0] );
        return( 1 );
    }

    (void) get_real_argument( 1.0, &line_thickness );

    if( input_graphics_file( src_filename,
                             &format, &n_objects, &objects ) != OK )
        return( 1 );

    for_less( i, 0, n_objects )
        set_object_colour( objects[i], colour );

    if( output_graphics_file( dest_filename, format, n_objects, objects ) != OK)
        return( 1 );

    delete_object_list( n_objects, objects );

    return( 0 );
}