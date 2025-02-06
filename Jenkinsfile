pipeline{
    agent{
        node{
            label 'docker-python'
        }
    }
    stages{
        stage('Build'){
            steps{
                echo 'Build'
                sh'''
                git stash
                git checkout request
                git pull
                '''
            }
        }
        stage('Test'){
            steps{
                echo 'Test'
                sh'''
                make re
                ./webserv & curl localhost:1800
                '''
            }
        }
        stage('Deploy'){
            steps{
                echo 'Deploy'
            }
        }
    }
}
