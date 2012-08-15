<?php

class facetly_api
{
    function setConsumer($key, $secret)
    {
        $this->key    = $key;
        $this->secret = $secret;
    }
    
    function setServer($server, $async = FALSE)
    {
        $this->server = $server;
        $this->async = $async;
    }
    function setBaseUrl($baseurl)
    {
        $this->baseurl = $baseurl;
    }
    
    function templateUpdate($tplsearch, $tplfacet, $tplpage = '')
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret,
            "tplsearch" => $tplsearch,
            "tplfacet" => $tplfacet,
            "tplpage" => $tplpage
        );
        $path   = "template/update";
        return $this->call($path, $data, 'POST');
    }
    
    function templateSelect()
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret
        );
        $path   = "template/select";
        return json_decode($this->call($path, $data, 'POST'));
    }
    
    function productDelete($id)
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret,
            "id" => $id
        );
        $path   = "product/delete";
        return $this->call($path, $data, 'POST');
    }
    
    function productTruncate()
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret
        );
        $path   = "product/truncate";
        return $this->call($path, $data, 'POST');
    }
    
    function productUpdate($items)
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret
        );
        $data   = array_merge($data, $items);
        $path   = "product/update";
        return $this->call($path, $data, 'POST');
    }
    
    function searchProduct($query, $filter, $searchtype)
    {
        $baseurl = $this->baseurl;
        $key     = $this->key;
        $data    = array(
            "key" => $key,
            "limit" => 3,
            "searchtype" => $searchtype,
            "baseurl" => $baseurl
        );
        
        if (!empty($query)) {
            $data['query'] = $query;
        }
        $data = array_merge($data, $filter);
        $path = "search/product";
        return json_decode($this->call($path, $data, 'GET'));
    }
    
    function searchHtml($query, $filter)
    {
        $baseurl = $this->baseurl;
        $key     = $this->key;
        $data    = array(
            "key" => $key,
            "limit" => 3,
            "baseurl" => $baseurl
        );
        
        if (!empty($query)) {
            $data['query'] = $query;
        }
        $data = array_merge($data, $filter);
        $path = "search/html";
        return $this->call($path, $data, 'GET');
    }
    
    function searchAutoComplete($query)
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "query" => $query
        );
        
        $path = "search/autocomplete";
        return json_decode($this->call($path, $data, 'GET'));
    }
    
    function reportQuery($from, $to, $query = "")
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret,
            "fromdate" => $from,
            "todate" => $to,
            "query" => "keywords_token:" . $query
        );
        $path   = "report/query";
        return json_decode($this->call($path, $data, 'POST'));
    }
    
    function reportTrend($from, $to, $query = "", $field = "keywords_token")
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret,
            "fromdate" => $from,
            "todate" => $to,
            "query" => $query,
            "size" => 5, // size of facets
            "field" => $field //selected field for facets
        );
        $path   = "report/trend";
        return json_decode($this->call($path, $data, 'POST'));
    }
    
    function reportStats()
    {
        $path = "report/stats";
        return json_decode($this->call($path, "", 'POST'));
    }
    
    function fieldSelect()
    {
        $key    = $this->key;
        $secret = $this->secret;
        $data   = array(
            "key" => $key,
            "secret" => $secret
        );
        $path   = "field/select";
        return json_decode($this->call($path, $data, 'POST'));
    }
    
    function call($path, $data, $method)
    {
        if (!$this->server)
            throw new Exception('$this->server needs a value');
        $data = http_build_query($data, '', '&');
        //replace multiple values [0]..[n] to [], thanks to http://www.php.net/manual/en/function.http-build-query.php#78603
        $data = preg_replace('/%5B(?:[0-9]|[1-9][0-9]+)%5D=/', '[]=', $data);
        $path = $this->server . "/" . $path;
        
        if ($this->async) {
          $this->curl_post_async($path, $data);
        } else {
        
        if ($method == 'POST') {
            $Curl_Session = curl_init($path);
            curl_setopt($Curl_Session, CURLOPT_POST, 1);
            curl_setopt($Curl_Session, CURLOPT_POSTFIELDS, $data);
        } else if ($method == 'GET') {
            $Curl_Session = curl_init($path . '?' . $data);
        }        
        
        curl_setopt($Curl_Session, CURLOPT_HEADER, FALSE);
        curl_setopt($Curl_Session, CURLOPT_RETURNTRANSFER, 1);
        curl_setopt($Curl_Session, CURLOPT_CUSTOMREQUEST, $method);
        curl_setopt($Curl_Session, CURLOPT_ENCODING, 1);
        curl_setopt($Curl_Session, CURLOPT_FOLLOWLOCATION, 1);
        curl_setopt($Curl_Session, CURLOPT_HTTPHEADER, array(
            "Accept-Encoding: gzip"
        ));
        $output = curl_exec($Curl_Session);
        $header = curl_getinfo($Curl_Session);
        
        if ($header['http_code'] != 200)
            throw new Exception($output);
        curl_close($Curl_Session);
        
        $this->output = $output;
        return $this->output;
        }
    }

    // curl async, thanks to http://petewarden.typepad.com/searchbrowser/2008/06/how-to-post-an.html
    function curl_post_async($url, $post_string)
    {    
        $parts=parse_url($url);

        $fp = fsockopen($parts['host'], 
            isset($parts['port'])?$parts['port']:80, 
            $errno, $errstr, 30);

        $out = "POST ".$parts['path']." HTTP/1.1\r\n";
        $out.= "Host: ".$parts['host']."\r\n";
        $out.= "Content-Type: application/x-www-form-urlencoded\r\n";
        $out.= "Content-Length: ".strlen($post_string)."\r\n";
        $out.= "Connection: Close\r\n\r\n";
        if (isset($post_string)) $out.= $post_string;

        fwrite($fp, $out);
        fclose($fp);
    }        
}

