<?php

namespace Walkthrough\Authenticator;

use Guzzle\Plugin\Oauth\OauthPlugin;
use Walkthrough\Authenticator;

include_once 'abstract.inc';

class Oauth extends WalkthroughAuthenticator {
  protected $consumerKey;
  protected $consumerSecret;

  /**
   * Logs in via 2 legged OpenAuth.
   */
  public function login() {
    $client = $this->getClient();

    // Get token.
    $response = $client->post($this->endpoint . '/user/token')->send()->json();
    $token = $response['token'];

    // Get user.
    $response = $client->post($this->endpoint . '/system/connect', array(), array('token' => $token))
      ->send()
      ->json();

    $this->drupalUser = $response;

    if ($this->drupalUser['user']['uid'] == 0) {
      throw new NotAuthenticatedException();
    }

    return $response;
  }

  /**
   * @param string $consumerKey
   */
  public function setConsumerKey($consumerKey) {
    $this->consumerKey = $consumerKey;
  }

  /**
   * @return string
   */
  public function getConsumerKey() {
    return $this->consumerKey;
  }

  /**
   * @param string $consumerSecret
   */
  public function setConsumerSecret($consumerSecret) {
    $this->consumerSecret = $consumerSecret;
  }

  /**
   * @return string
   */
  public function getConsumerSecret() {
    return $this->consumerSecret;
  }

  /**
   * Prepare the Guzzle http client.
   *
   * Also populates the cookies if the authentication was successful.
   *
   * @return \Guzzle\Http\Client
   */
  public function getClient() {
    $client = parent::getClient();

    $oauth = new OauthPlugin(array(
      'consumer_key' => $this->getConsumerKey(),
      'consumer_secret' => $this->getConsumerSecret(),
    ));

    $client->addSubscriber($oauth);

    return $client;
  }
}
