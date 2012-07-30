<?php

	function facetly_search_template_page() {		
		global $language; 
		facetly_add_js();
		module_load_include('inc','facetly','facetly.common');
		drupal_add_css(drupal_get_path('module', 'facetly') . '/css/facetly.css');

		//$output = drupal_get_form('facetly_search_form');
		$output = '';
		//if ( !empty($_GET['search'] )) {
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
	  		
	  		//if ( isset($_GET['page']) ) $filter['page'] = $_GET['page']+1;
	  		//else $filter['page'] = 1;
	  		
	  		$searchtype = "html";
			$var = facetly_search_result($query, $filter, $searchtype);
			if (!empty($var)) {
				$output .= '<div id="facetly_result">'. $var->results.'</div>';
				//$total = $var->htmltotal;
				//$output .= theme('facetly_pager', $total, $get['limit']);
			}
			//print_r($var);
		//}
		return  $output;	
	}

	function facetly_search_page(){
	  drupal_add_js(drupal_get_path('module', 'facetly') . '/js/jquery.address.js');
	  drupal_add_js(drupal_get_path('module', 'facetly') . '/js/facetly.js');
	  module_load_include('inc','facetly','facetly.common');
    
		$settings = array(
		  'facetly_state' => url($_GET['q']),      
		);	  
		drupal_add_js($settings, 'setting');
	
		drupal_add_css(drupal_get_path('module', 'facetly') . '/css/facetly.css');
		$output = drupal_get_form('facetly_search_form');
		//if ( !empty($_GET['search'] )) {
			$url = explode('?', $_SERVER['REQUEST_URI']);
			$get = array();    
	  		parse_str($url[1], $get);
			if ( empty($get['limit']) ) $get['limit'] = variable_get('facetly_limit', '');
		
			$query = "";
			if (!empty($_GET['query'])) $query = $_GET['query'];
			$filter = $get; 
			unset($filter['query']);
			$time_start = microtime();
			
			$filter['session'] = session_id();
			$filter['ip'] = ip_address();
	  		$filter['user_agent'] = $_SERVER['HTTP_USER_AGENT'];
	  		//if ( isset($_GET['page']) ) $filter['page'] = $_GET['page']+1;
	  		//else $filter['pages'] = 1;
	  		
	  		//print_r($filter);
	  		$searchtype = "json";
			$var = facetly_search_result($query, $filter, $searchtype);
			//print_r($var);		
			if ( !empty($var) ) {
				$item = $var->items;

				$time_end = microtime();
				$time = $time_end - $time_start;
				$total = $var->total;
				if (isset($_GET['page'])) {
					$offset = (int)($_GET['page'] * $get['limit']);
					//$from = $offset+1;
					$from = $offset;
					$last = $offset+$get['limit'];
					$size = $offset+$get['limit'];
					if ( $size > $total ) $size = $total;
					if ( $last > $total ) $results['size'] = $total; 
				}
				else {
					$from = 0;
					$size = $get['limit'];
				} 
				$output .= "Results ". $from. " - ". $size. " of ". $total ." for <b>". $query. "</b> in time: ". $time. " seconds <br/><br/>";
				$page = theme('facetly_pager', $total, $get['limit']);
				$item_list = array();
		
				$itemIterator = new ArrayIterator($item);
				foreach ($itemIterator as $itemkey => &$itemvalue) {
					$items = (array)$itemvalue;
					//print_r($itemvalue);
					$item_list[] = theme('facetly_item', $items);
				}
				$facetly_field_key = variable_get('facetly_field_key', array());
				$output .= theme('item_list', $item_list, '', 'ul',array('class' => 'facetly_result'));
				$output .= $page;
			}
			else {
					$output .= "No Result Found";
			}
		//}
		return  '<div id="facetly_result"> '. $output . '</div>';
	}


function facetly_autocomplete_search_page()
{
    module_load_include('inc','facetly','facetly.common');
    $matches = array();
    $string  = $_GET['query'];
    
    $limit = 10;
    if (!empty($string)) {
        $suggestions = array();
        $filter      = array();
        $var         = facetly_history_result($string);	
        if (!empty($var)) {
            $item = $var->suggestions;
            foreach ($item as $key => $value) {
              $suggestions[] = $value;
            }
        }
    }
    
    $matches = array(
        'query' => $string,
        'suggestions' => $suggestions
        //'data' => $data,
    );
    drupal_json($matches);
}
