<?php
/**
 * @file
 * AWS SQS module admin functions.
 */

/**
 * Menu callback for admin form.
 *
 * @see aws_sqs_settings_form_validate()
 */
function aws_sqs_settings_form() {
  $form['credentials'] = array(
    '#type' => 'fieldset',
    '#title' => t('AWS credentials'),
    '#description' => t('Follow the instructions to set up your AWS credentials !here.',
      array('!here' => l('here', 'http://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSGettingStartedGuide/AWSCredentials.html'))),
  );
  $form['credentials']['aws_sqs_aws_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Key ID'),
    '#default_value' => variable_get('aws_sqs_aws_key', ''),
    '#required' => TRUE,
    '#description' => t('Amazon Web Services Key.'),
  );
  $form['credentials']['aws_sqs_aws_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret Access Key'),
    '#default_value' => variable_get('aws_sqs_aws_secret', ''),
    '#required' => TRUE,
    '#description' => t('Amazon Web Services Secret Key.'),
  );

  $form['aws_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('AWS settings'),
  );
  $seconds = range(0, 20);
  $form['aws_settings']['aws_sqs_waittimeseconds'] = array(
    '#type' => 'select',
    '#title' => t('Wait Time'),
    '#default_value' => variable_get('aws_sqs_waittimeseconds', 1),
    '#options' => $seconds,
    '#description' => t('How long do you want to stay connected to AWS waiting for a response (seconds)? If a queue is empty, the connection will stay open for up to 20 seconds. If something arrives in the queue, it is returned as soon as it is received. AWS SQS charges per request. Long connections that stay open waiting for data to arrive are cheaper than polling SQS constantly to check for data. Long polling can also consume more resources on your server (think about the difference between running a task every minute that takes a second to complete versus running a task every minute that stays connected for up to 20 seconds every time waiting for jobs to come in). !more', array('!more' => l('Read more about long polling here.', 'http://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/sqs-long-polling.html#sqs-long-polling-query-api'))),
  );
  $form['aws_settings']['aws_sqs_claimtimeout'] = array(
    '#type' => 'textfield',
    '#title' => t("Claim Timeout / Visibility Timeout"),
    '#default_value' => variable_get('aws_sqs_claimtimeout', 60),
    '#size' => 15,
    '#description' => t("When an item is claimed from the queue by a worker, how long should the item be hidden from other workers (seconds)? Note: If the item is not deleted before the end of this time, it will become visible to other workers and available to be claimed again. Note also: 12 hours (43,200 seconds) is the maximum amount of time for which an item can be claimed. !more", array('!more' => l('Read more about visibility timeouts here.', 'http://docs.aws.amazon.com/AWSSimpleQueueService/latest/SQSDeveloperGuide/AboutVT.html'))),
  );

  $form['aws_settings']['aws_sqs_region'] = array(
    '#type' => 'select',
    '#title' => t('Region'),
    '#default_value' => variable_get('aws_sqs_region', 'us-east-1'),
    '#options' => array(
      'ap-northeast-1' => 'ap-northeast-1',
      'ap-southeast-1' => 'ap-southeast-1',
      'ap-southeast-2' => 'ap-southeast-2',
      'eu-central-1' => 'eu-central-1',
      'eu-west-1' => 'eu-west-1',
      'sa-east-1' => 'sa-east-1',
      'us-east-1' => 'us-east-1',
      'us-west-1' => 'us-west-1',
      'us-west-2' => 'us-west-2',
    ),
    '#required' => TRUE,
    '#description' => t('Amazon Web Services Region.'),
  );

  $form['drupal_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Drupal settings'),
  );

  $queue_classes = aws_sqs_get_queue_classes();
  $form['drupal_settings']['queue_default_class'] = array(
    '#type' => 'select',
    '#title' => t('Default Queue'),
    '#default_value' => variable_get('queue_default_class', 'SystemQueue'),
    '#description' => t('Set the default queue class. If you select AwsSqsQueue here, AWS SQS will be used anytime a queue is instantiated via DrupalQueue:get($name).'),
    '#options' => array_combine($queue_classes, $queue_classes),
  );

  $form['drupal_settings']['aws_sqs_queue_name_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Queue name prefix'),
    '#default_value' => variable_get('aws_sqs_queue_name_prefix', ''),
    '#size' => 30,
    '#maxlength' => AWS_SQS_QUEUE_NAME_PREFIX_MAXLENGTH,
    '#description' => t('Optionally prefix queue names to prevent cross-contamination between sites or environments. This is critically important if you use a single AWS account for multiple applications (e.g. example.com, something.com, and somethingelse.com) or for multiple different instances of the same application (e.g. dev.example.com, stage.example.com, example.com). Use any combination of alphanumeric characters, hyphens (-), and underscores (_), e.g., "example-com_" or "example-com_dev_". Keep in mind that queue names are limited to 80 characters, including prefix.'),
  );

  return system_settings_form($form);
}

/**
 * Form validation handler for aws_sqs_settings_form().
 */
function aws_sqs_settings_form_validate($form, &$form_state) {
  $aws_sqs_queue_name_prefix = $form_state['values']['aws_sqs_queue_name_prefix'];

  if (preg_match('/^[0-9a-zA-Z\-_]*$/', $aws_sqs_queue_name_prefix) === 0) {
    form_set_error('aws_sqs_queue_name_prefix', t('The queue name prefix can only contain alphanumeric characters, hyphens (-), and underscores (_).'));
  }
}
