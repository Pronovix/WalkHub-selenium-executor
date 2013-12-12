<?php
require 'vendor/autoload.php';

$command_line = new Commando\Command();

$command_line->option('username')
  ->aka('u')
  ->describedAs('Drupal user name')
  ->require();

$command_line->option('password')
  ->aka('p')
  ->describedAs('Drupal password')
  ->require();

$command_line->option('walkhub_url')
  ->aka('w')
  ->describedAs('Walkhub url.')
  ->require();

$command_line->argument()
  ->referToAs('Action')
  ->describe('Action to perform. Possible options:
  * status
    Test connection to the walkhub.

  * get_queue
    Get the screenshot queue.

  * get_phpunit [uuid]
    Get the pphunit export for a walkthrough.');

$command_line->option('debug')
  ->aka('d')
  ->describedAs('Debug mode')
  ->boolean();

function command__status($command_line) {
  $endpoint = _get_endpoint($command_line);

  try {
    $user = login($endpoint, $command_line['username'], $command_line['password']);
  } catch (\Guzzle\Http\Exception\ClientErrorResponseException $e) {
    $response = $e->getResponse();
    echo 'Error: [' . $response->getStatusCode() . '] ' . $response->getReasonPhrase() . "\n";
    exit(1);
  }

  echo "Ok.\n";
}

function command__get_queue($command_line) {
  $endpoint = _get_endpoint($command_line);

  //$user = login($endpoint, $command_line['username'], $command_line['password']);
  $client = new Guzzle\Http\Client($endpoint);
  $response = $client->get('walkhub-walkthrough-screening-queue')->send()->json();
  var_dump($response);
}

function command__get_phpunit($command_line) {
  $endpoint = _get_endpoint($command_line);

  $client = new Guzzle\Http\Client($endpoint);
  $response = $client->get('walkthrough-phpunit/' . $command_line[1])->send()->json();
  echo $response[0];
}

function _get_endpoint($command_line) {
  return $command_line['walkhub_url'] . '/api/v2/';
}

function login($endpoint, $username, $password) {
  $client = new Guzzle\Http\Client($endpoint);

  $response = $client->post('user/login', null, array(
    'username' => $username,
    'password' => $password
  ))->send()->json();

  return $response;
}

$function_name = 'command__' . $command_line[0];
if (function_exists($function_name)) {
  call_user_func($function_name, $command_line);
}
