<?php

namespace Walkthrough\Authenticator;

use Walkthrough\Authenticator;

include_once 'abstract.inc';

class Basic extends WalkthroughAuthenticator {
  protected $username;
  protected $password;

  public function login() {
    $client = $this->getClient();
    $response = $client->post('user/login', NULL, array(
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
}