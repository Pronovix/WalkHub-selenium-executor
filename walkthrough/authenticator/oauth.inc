<?php

namespace Walkthrough\Authenticator;
use Walkthrough\Authenticator;
use Guzzle\Http\Client;
use Guzzle\Plugin\Oauth\OauthPlugin;

include_once 'abstract.inc';

class Oauth extends WalkthroughAuthenticator {
  protected $consumerKey;
  protected $consumerSecret;

  /**
   * @return array
   */
  public function login() {
    $client = new Client($this->endpoint);

    $oauth = new OauthPlugin(array(
      'consumer_key'    => $this->getConsumerKey(),
      'consumer_secret' => $this->getConsumerSecret(),
    ));

    $client->addSubscriber($oauth);

    // Get token
    $response = $client->post($this->endpoint . '/user/token')->send()->json();
    $token = $response['token'];

    // Get user.
    $response = $client->post($this->endpoint . '/system/connect', array(), array('token' => $token))
      ->send()
      ->json();

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

}
