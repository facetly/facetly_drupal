<?php

function facetly_reindex( $node ) {
    $productId = $node->nid;
    $facetly_field_key = variable_get( 'facetly_field_key', array ());

    $items = array();
    foreach ( $facetly_field_key as $key => $value ) {
        $deletekey = FALSE;
        if ( strpos( $value, 'node_' ) !== FALSE ) {
            switch ( $value ) {
                case 'node_nid':
                    $items[$key] = $node->nid;
                    break;
                case 'node_title':
                    $items[$key] = strip_tags( $node->title );
                    break;
                case 'node_body':
                    $items[$key] = strip_tags( $node->body );
                    break;
                case 'node_created':
                    $items[$key] = $node->created;
                    break;
                case 'node_url':
                    $items[$key] = url( "node/" . $node->nid, array(
                         "absolute" => TRUE 
                    ) );
                    break;
                case 'node_sell_price':
                    $items[$key] = $node->sell_price;
                    break;

            }
        }
        if ( strpos( $value, 'field_' ) !== FALSE ) {
            static $widget_module;

            list( $value, $additional ) = explode( ":", $value );

            if ( empty( $widget_type[$node->type][$value] ) ) {
                $widget_module[$node->type][$value] = db_result( db_query( "SELECT widget_module FROM {content_node_field_instance} WHERE type_name='%s' AND field_name='%s'", $node->type, $value ) );
            }

            switch ( $widget_module[$node->type][$value] ) {
                case 'imagefield':
                    $items[$key] = imagecache_create_url( $additional, $node->{$value}[0]['filepath'] );
                    $deletekey = TRUE;
                    break;
                case 'content_taxonomy_options':
                    foreach ( $node->{$value} as $value2 ) {
                        $term = taxonomy_get_term( $value2['value'] );
                        $items[$key][] = $term->name;
                        $deletekey = TRUE;
                    }
                    break;
                case 'date':
                    list( $date, $time ) = explode( " ", $node->{$value}[0]['value'] );
                    $items[$key] = $date;
                    $deletekey = TRUE;
                    break;
                default:
                    $items[$key] = strip_tags( $node->{$value}[0]['value'] );
                    $deletekey = TRUE;
                    break;
            }
        }
        if ( $deletekey ) {
            unset( $facetly_field_key[$key] );
        }
    }
    // assumption: all existing key on facetly field key are vocabulary			
    foreach ( $facetly_field_key as $key => $value ) {
        if ( !empty( $node->taxonomy ) ) {
            foreach ( $node->taxonomy as $key2 => $value2 ) {
                $vocabulary_load = taxonomy_vocabulary_load( $value2->vid );
                $vocabulary_name = $vocabulary_load->name;
                if ( $vocabulary_name == $value ) {
                    $tid = $value2->tid;
                    $parents = taxonomy_get_parents_all( $tid );
                    $parents_all = array();
                    foreach ( $parents as $pkey => $pval ) {
                        $parents_all[] = $pval->name;
                    }
                    krsort( $parents_all );
                    $items[$key][] = join( ";", $parents_all );
                }
            }
        }
    }

    $items['created'] = $items['created'] * 1000;
    $items['price'] = ( ( is_numeric( $items['price'] ) ) ? $items['price'] : (int) $items['price'] );

    $facetly = facetly_api_init();
    try {
        $facetly->productUpdate( $items );
    }
    catch ( Exception $e ) {
        drupal_set_message( t( $e->getMessage() ), 'error', TRUE );
    }
}

function facetly_search_result( $query, $filter, $searchtype ) {
    static $var;
    if ( empty( $var ) ) {
        $facetly = facetly_api_init();

        $url = url( "find", array(
            'query' => variable_get( 'facetly_extra_vars', '' ) 
        ) );
        $post = array(
            //"limit" => 3,					
            "baseurl" => $url 
        );

        if ( !empty( $query ) ) {
            $post['query'] = $query;
        }

        $post = array_merge( $post, $filter );
        try {
            $var = $facetly->searchProduct( $query, $post, 'html' );
        }
        catch ( Exception $e ) {
            $var = new stdClass();
            $var->results = $e->getMessage();
        }
    }
    return $var;
}


function facetly_search_form( $form_state, $vars = array( 'title' => TRUE ) ) {
    $query = "";
    if ( !empty( $_GET['query'] ) )
        $query = $_GET['query'];

    if ( isset( $vars['title'] ) && $vars['title'] == TRUE ) {
        $title = t( 'Search' );
    } else {
        $title = '';
    }
    $form['query'] = array(
        '#type' => 'textfield',
        '#title' => $title,
        '#size' => 20,
        '#default_value' => $query,
        '#attributes' => array(
             'facetly' => 'on' 
        ) 
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t( 'Submit' ) 
    );
    $form['limit'] = array(
        '#type' => 'hidden',
        '#value' => variable_get( 'facetly_limit', '10' ) 
    );
    $form['#action'] = url( 'find', array(
        'query' => variable_get( 'facetly_extra_vars', '' ) 
    ) );
    $form['#method'] = 'GET';
    return $form;
}

function facetly_history_result( $query ) {
    $facetly = facetly_api_init();
    $var = $facetly->searchAutoComplete( $query );
    return $var;
}


function facetly_faceted_search() {
   $url = explode( '?', $_SERVER['REQUEST_URI'] );
    $get = array();

    if ( empty( $url[1] ) ) {
        $url[1] = '';
    }

    $query = "";
    $output = "";
    parse_str( $url[1], $get );
    if ( empty( $get['limit'] ) )
        $get['limit'] = variable_get( 'facetly_limit', '' );

    if ( !empty( $_GET['query'] ) ) {
        $query = $_GET['query'];
    }

    $filter = $get;
    unset( $filter['query'] );

    $category = "";
    $price = "";

    $searchtype = "html";
    $var = facetly_search_result( $query, $filter, $searchtype );
    if ( !empty( $var ) ) {
        if ( $searchtype == "json" ) {
            $group = $var->facets;
            $output = "";
            $request = $get;
            $request['query'] = $query;
            $query['limit'] = $get['limit'];
            foreach ( $group as $facetkey => &$facetvalue ) {
                $output .= "Facet: " . $facetkey;
                $request2 = $request;
                $terms = $facetvalue->facetTerms;
                foreach ( $terms as $termkey => &$termvalue ) {
                    if ( $termvalue->type == "terms" ) {
                        if ( empty( $_GET[$facetkey] ) ) {
                            unset( $request2[$facetkey] );
                        }
                        $query2[$facetkey] = $termvalue->name;
                        $output .= "<dd>" . l( $termvalue->name, "find", array(
                            'query' => $request2 
                        ) ) . " ($termvalue->count)";
                        if ( isset( $_GET[$facetkey] ) && $_GET[$facetkey] == $termvalue->name ) {
                            $get2 = $get;
                            unset( $get2[$facetkey] );
                            $output .= l( " X ", "find", array(
                                'query' => $get2 
                            ) );
                        }
                    } else {
                        $query2[$facetkey] = $termvalue->from . ":" . $termvalue->to;
                        $output .= "<dd>" . l( $termvalue->name, "find", array(
                             'query' => $query2 
                        ) ) . " ($termvalue->count)";
                        if ( isset( $_GET[$facetkey] ) && $_GET[$facetkey] == $request2[$facetkey] ) {
                            $get2 = $get;
                            unset( $get2[$facetkey] );
                            $output .= l( " X ", "find", array(
                                'query' => $get2 
                            ) );
                        }
                    }
                }
                $output .= "<br/></dd>";
            }
        } else {
            $output = $var->facets;
        }
    }
    
    if ( !empty( $output ) ) {
        $output = '<div id="facetly_facet">' . $output . '</div>';
    }
    return $output;
}