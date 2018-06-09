pipeline {
  agent any

  options {
    buildDiscarder( logRotator(artifactDaysToKeepStr: '90') )
    checkoutToSubdirectory('source')
    timeout(time: 1, unit: 'HOURS')
  }
  triggers { 
    pollSCM('H * * * *') 
  }

  stages {
    stage('CMake') { steps {
      dir("${env.WORKSPACE}/build") {
        sh '''cmake -DCMAKE_INSTALL_PREFIX=../install ../source'''
      }
    } }
    stage('Build') {
      steps {
        dir("${env.WORKSPACE}/build") {
          sh '''make install'''
        }
      }
    }
    stage('Artifacts') {
      steps {
          sh '''tar -cvzf OSGLauncher.tar.gz install/*'''
archiveArtifacts artifacts: 'OSGLauncher.tar.gz', fingerprint: true, onlyIfSuccessful: true
      }
    }
  }

  post {
    always {
emailext attachLog: true, body: '''${DEFAULT_CONTENT}''', compressLog: true, recipientProviders: [[$class: 'CulpritsRecipientProvider']], subject: '${DEFAULT_SUBJECT}', to: '${DEFAULT_RECIPIENTS}'
    }
  }
}

