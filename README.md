Facetly Drupal
==============

Install Facetly Module
----------------------

How to install Facetly Module in Drupal

1. Before Installing Facetly Module, make sure you already have these requirements:
    
	a. Any FTP program, such as WinSCP, FileZilla, etc.

	b. Drupal 6.0

	c. Ubercart Drupal Module, please download this module here (http://drupal.org/project/ubercart)

	d. Latest version of jQuery Update, please download this module here (http://drupal.org/project/jquery_update)

2. Download Facetly Module from our site (https://github.com/facetly/facetly_drupal) and upload it to your module folder using FTP program. Activate Facetly Module in your modules list.

3. After Facetly Module successfully installed in your Drupal, you will find Facetly Configuration in your Admin Menu and contain submenus: Facetly Settings, Fields, Reindex, and Template



Configure Facetly Module
------------------------

Now we are going to configure Facetly Module. There are several steps that you are need to do:

1. As we are already seen in picture above, there were 4 main configurations: Facetly configuration, Field, Template, and Reindex. We will start to configure Facetly Module from Facetly Configuration first. Here we will input Consumer Key, Consumer Secret, Server Name, Search Limit, and Additional Variable.

2. The next step is configure Field mapping. Go to Field Tab and we will see field mapping here. Please follow instruction in https://www.facetly.com/doc/field to set field mapping.

3. After you defined the field map, go to Template tab to set up template for your search page. You will see search template and facet template settings which will be displayed in your search page. You can find more details about template settings in https://www.facetly.com/doc/template

4. The final step is to reindex data in Reindex tab. This configuration is used to save all data in your store to our server, which will used as your search data. Click Reindex button to start the process. Please note: you should wait until process is complete and not move to other page, otherwise your data reindex will not completed and you must start from the beginning.

5. Set search box and facetly result into top left or right sidebar. Go to administer >> block then change position Facetly Module into top of left or right sidebar

6. Set facetly result only show in find pages, go to administer >> block >> configure then choose "show on only the listed pages" and type "find" in pages
