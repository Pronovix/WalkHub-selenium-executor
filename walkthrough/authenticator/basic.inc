<?php

namespace Walkthrough\Authenticator;
use Walkthrough\Authenticator;
use Guzzle\Http\Client;

include_once 'abstract.inc';

class Basic implements WalkthroughAuthenticator {
  protected $username;
  protected $password;
  protected $endpoint;

  public function login() {
    $client = new Client($this->endpoint);
    $response = $client->post('user/login', null, array(
      'username' => $this->username,
      'password' => $this->password,
    ))->send()->json();

    $this->drupalUser = $response;

    return $response;
  }

  /**
   * @param mixed $password
   */
  public function setPassword($password) {
    $this->password = $password;
  }

  /**
   * @return mixed
   */
  public function getPassword() {
    return $this->password;
  }

  /**
   * @param mixed $username
   */
  public function setUsername($username) {
    $this->username = $username;
  }

  /**
   * @return mixed
   */
  public function getUsername() {
    return $this->username;
  }

  /**
   * @param mixed $endpoint
   */
  public function setEndpoint($endpoint) {
    $this->endpoint = $endpoint;
  }
  protected $drupalUser;
}