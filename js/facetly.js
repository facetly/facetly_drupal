Drupal.behaviors.sphinx_autocomplete = function() {
  jQuery('input[facetly="on"]').each(function(index, elm) {
    var input = jQuery(this);
    var autosubmit = true;    
    var nocache = false;
    var gmap;
    /*if (input.attr('autosubmit') == 'no') {
      autosubmit = false;
    } else {
      autosubmit = true;
    }
    
    if (input.attr('nocache') == 'yes') {
      nocache = true;
    } else {
      nocache = false;
    }
    
    if (input.attr('gmap') == 'yes') {
      gmap = true;
    } else {
      gmap = false;
    } */
    
    //var xhr;          
    
    var delay = (function(){
      var timer = 0;
      return function(callback, ms){
        clearTimeout (timer);
        timer = setTimeout(callback, ms);
      };
    })();

                                             
    jQuery(input).keyup(function() {   
      delay(function() {      
        params = {};
        facetly_server = Drupal.settings.facetly_server;
        params.key = Drupal.settings.facetly_key;
        params.baseurl = Drupal.settings.facetly_baseurl;
        params.searchtype = "html";
        params.limit = Drupal.settings.facetly_limit;          
        if (jQuery(input).val() != "") {
          params.query = jQuery(input).val();
        }
        
        jQuery.ajax({
          url: facetly_server + "/search/product",
          dataType: "jsonp",
          type: "GET",
          data : params,
          success: function(data) {		
            jQuery('#facetly_result').html(data.results);
            jQuery('#facetly_facet').html(data.facets);
            jQuery(document).trigger("facetly_loaded");
          }
        });
      }, 300);
    });
        
    var serviceUrl= Drupal.settings.facetly_server+'/search/autocomplete';
    var params={
      "key" : Drupal.settings.facetly_key,
    }
    jQuery(input).fautocomplete({
      autoSubmit: autosubmit,
      params: params, 
      noCache: nocache,
      gmap: gmap,
      serviceUrl:serviceUrl,  
      jsonp: true,          
    });                        
  });
  




    var init = true, 
        state = window.history.pushState !== undefined;
    
    // Handles response
    var handler = function(data) {
       // jQuery('title').html(jQuery('title', 'test test').html());
        jQuery('#facetly_result').html(data.results);
        jQuery('#facetly_result').show();
        jQuery('#facetly_facet').html(data.facets);
        jQuery('#facetly_facet').show();
        //jfacetly.address.title(/>([^<]*)<\/title/.exec(data)[1]);
    };	
    
    
    if (jQuery('.pager a, #facetly_facet a')) {
    jQuery.address.state(Drupal.settings.facetly_state).init(function() {
        // Initializes the plugin
        jQuery('.pager a, #facetly_facet a').address();
        
    }).change(function(event) {
        // Selects the proper navigation link
        jQuery('.pager a').each(function() {
            if (jQuery(this).attr('href') == (jQuery.address.state() + event.path)) {
                jQuery(this).parent().addClass('pager-current').focus();
            } else {
                jQuery(this).parent().removeClass('pager-current');
            }
        });
        
        if (state && init) {
        
            init = false;
            jQuery(document).trigger("facetly_loaded");
        
        } else {
                    
           params = {};
           // fix bug [], replace %5B AND %5D
           for (var i = 0; i < event.parameterNames.length; i++) {
             var key = event.parameterNames[i];
             var newkey = decodeURIComponent(key);
             if (typeof event.parameters[key] == "string") {             	
             	params[newkey] = decodeURIComponent(event.parameters[key]).replace(/\+/g, ' ');
             } else {
             	var values = [];
             	var value_temp = event.parameters[key];
             	for (var j = 0; j < value_temp.length; j++) {             		
             		values[j] = decodeURIComponent(value_temp[j]).replace(/\+/g, ' ');
             	}
             	params[newkey] = values;
             }
           }
           
           facetly_server = Drupal.settings.facetly_server;
           params["key"] = Drupal.settings.facetly_key;
           params["key"] = Drupal.settings.facetly_key;
           params["baseurl"] = Drupal.settings.facetly_baseurl;
           params["searchtype"] = "html";
           
           jQuery.ajax({
             url: facetly_server + "/search/product",
             dataType: "jsonp",
             type: "GET",
             data : params,
             success: function(data, textStatus, XMLHttpRequest) {		
              handler(data);
              jQuery(document).trigger("facetly_loaded");
             }
           });
        }

    });
    } 
}	

