<?php

namespace Walkthrough\Authenticator;

use Walkthrough\Authenticator;

include_once 'abstract.inc';

class Anonymous extends WalkthroughAuthenticator {
  public function login() {
    $client = $this->getClient();

    // Get token.
    $response = $client->post($this->endpoint . '/user/token')->send()->json();
    $token = $response['token'];

    // Get the Anonymous user.
    $response = $client->post($this->endpoint . '/system/connect', array(), array('token' => $token))
      ->send()
      ->json();

    $this->drupalUser = $response;

    return $response;
  }
}