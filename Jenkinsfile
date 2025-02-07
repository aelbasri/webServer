pipeline{
    agent{
        node{
            label 'docker-python'
        }
    }
    stages{
        stage('Build'){
            steps{
                withCredentials(['c8604461-0dd1-47d7-9af8-faaeefaab89f'])
                {
                    echo 'Build'
                    sh'''
                    git stash
                    git checkout request
                    git pull
                    '''
                }
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
