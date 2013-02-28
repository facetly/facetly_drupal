<?php

	function facetly_search_template_page() {		
		global $language; 
		facetly_add_js();
		module_load_include('inc','facetly','facetly.common');
		drupal_add_css(drupal_get_path('module', 'facetly') . '/css/facetly.css');

		$output = '';
		if (isset($_GET['query'])) {
			$query = $_GET['query'];
		  	drupal_set_title(t('Search !query', array('!query' => $query)));
		  
		}			
		$url = explode('?', $_SERVER['REQUEST_URI']);
		$get = array();    
  		parse_str($url[1], $get);
		if ( empty($get['limit']) ) $get['limit'] = variable_get('facetly_limit', '');
	
		$query = "";
		if (!empty($_GET['query'])) $query = $_GET['query'];
		$filter = $get; 
		unset($filter['query']);
		
		if (!empty($_GET['flag'])) $filter['flag'] = $_GET['flag'];
		
		if (module_exists('locale')) {
		  	$facetly_field_key = variable_get('facetly_field_key', array());
		  	if (!isset($_GET['language'])) {
		    	switch ($facetly_field_key['language']) {
		      		case 'node_language_native':
		        		$filter['language'] = $language->native;
		      			break;
		      		case 'node_language_id':
		        		$filter['language'] = $language->language;
		      			break;
		    		}	  		  
		  	} else {
		    	$filter['language'] = $_GET['language'];
		  	}
  		}			
		
		$filter['session'] = session_id();
		$filter['ip'] = ip_address();
  		$filter['user_agent'] = $_SERVER['HTTP_USER_AGENT'];
  		
  		$searchtype = "html";
		$var = facetly_search_result($query, $filter, $searchtype);
		if (!empty($var)) {
			$output .= '<div id="facetly_result">'. $var->results.'</div>';
		}
		return  $output;	
	}

	function facetly_autocomplete_search_page() {
	    $matches = array();
	    $query = $_GET['query'];
	    
	    $limit = 10;
	    if (!empty($query)) {
	        $suggestions = array();
	        $filter = array();

	        $facetly = facetly_api_init();
        	$var = $facetly->searchAutoComplete($query);
	        if (!empty($var)) {
	            $item = $var->suggestions;
	            foreach ($item as $key => $value) {
	              $suggestions[] = $value;
	            }
	        }
	    }
	    
	    $matches = array(
	        'query' => $query,
	        'suggestions' => $suggestions
	    );
	    drupal_json($matches);
	}
